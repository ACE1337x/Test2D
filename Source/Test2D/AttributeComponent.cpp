// Fill out your copyright notice in the Description page of Project Settings.

#include "Test2D.h"
#include "AttributeComponent.h"
#include "Test2DEnums.h"
#include "AbstractSkill.h"
#include "Equippable.h"
#include "Net/UnrealNetwork.h"

//#include "HpBarActor.h"
//#include "../Source/Runtime/Engine/Classes/Components/SceneComponent.h"

UAttributeComponent::UAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
	// ...
	//bReplicates = true;
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (GetOwner()->HasAuthority())
	{
		totalStats = UStructHelperFunctions::AddAttributes(baseStats, totalStats);
		currJumps = totalStats.MaxJumps;
	}
	if (GetOwner()->HasAuthority())
		Respawn();
}
void UAttributeComponent::Init()
{
}

// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, Alive ? FString("Alive") : FString("Dead"));
	if (!GetOwner()->HasAuthority())
		return;

	if (Alive)
	{
		hpRegenTimer -= DeltaTime;
		if (hpRegenTimer <= 0.0f)
		{
			Heal(totalStats.HpRegeneration * totalStats.MaxHP * 0.2f);
			hpRegenTimer = 1.0f;
		}
	}
	else
	{
		if (currRespawnTime == MaxRespawnTime) // just died
		{
			clearStatuses();
		}

		currRespawnTime -= DeltaTime;
		if (currRespawnTime <= 0.0f)
		{
			Respawn();
		}
	}

	// ...
}

void UAttributeComponent::TakeDamage_Implementation(
	UAttributeComponent * damageCauser,
	UObject * damageSource,
	int SkillUseID,
	EDmgType DmgType,
	FAbstractSkillData otherSkillData,
	FPlayerAttributes otherAttributes,
	FVector locationOfCollision,
	TSubclassOf<ADamageTextActor> dmgTextActor)
{
	if (!GetOwner()->HasAuthority())
		return;

	if (!Alive)
		return;
	float damageBeforeDefense;
	damageBeforeDefense = otherSkillData.skillData.skillDamageCoefficient * otherAttributes.Damage;
	damageBeforeDefense = FMath::CeilToFloat(damageBeforeDefense);

	float totalCritChance = otherSkillData.skillData.skillIncreasedCritChance + otherAttributes.Critical_Chance;
	float totalCritDamage = otherSkillData.skillData.skillIncreasedCritDamage + otherAttributes.Critical_Damage;
	float totalLifeSteal = otherSkillData.skillData.skillIncreasedLifesteal + otherAttributes.LifeSteal;

	float pray2RnGesus = FMath::FRandRange(0, 100.0f) / 100.0f;
	bool criticalHit = totalCritChance >= pray2RnGesus;

	if (criticalHit)
		damageBeforeDefense *= totalCritDamage;

	float damageAfterDefense = FMath::CeilToInt(damageBeforeDefense);

	float finalDefense = totalStats.Damage_Reduction * (1 - otherAttributes.Defense_Penetration);

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::SanitizeFloat(damageAfterDefense * ((100.0f - finalDefense) / 100.0f)));

	damageAfterDefense = damageAfterDefense * (1.0f - finalDefense);

	if (damageAfterDefense < 0.0f)
		damageAfterDefense = 0.0f;

	EDmgType dmgType = DmgType;

	if (dmgTextActor == nullptr)
		dmgTextActor = damageTextActorStyle;

	if (bIsInvuln)
		dmgType = EDmgType::DT_Invulnerable;

	if (bIsEvading && otherSkillData.skillData.isEvadable)
	{
		dmgType = EDmgType::DT_Evade;
		onEvadeDamageDelegate.Broadcast(damageAfterDefense, damageBeforeDefense, damageCauser);
	}

	if (!bIsInvuln && !bIsEvading)
	{
		currHp -= damageAfterDefense;

		if (currHp <= 0 && Alive)
		{
			Die();
		}

		if (locationOfCollision == FVector::ZeroVector)
			locationOfCollision = GetOwner()->GetActorLocation();

		if (damageCauser)
		{
			damageCauser->Heal(damageAfterDefense * totalLifeSteal);
			onTakeDamageDelegate.Broadcast(damageAfterDefense, damageBeforeDefense, damageCauser);
		}

		FVector kb = otherSkillData.skillData.knockBack;

		if (damageSource && damageSource->GetClass()->IsChildOf(UAbstractSkill::StaticClass()))
		{
			UAbstractSkill * otherSkill = (UAbstractSkill*)damageSource;

			if (FVector(GetOwner()->GetActorLocation() - otherSkill->initialSkillUseLoc).X < 0)
				kb.X = -kb.X;

			if (otherSkillData.skillInteractionInfo.knockbackUseVelocity)
				kb += otherSkill->GetOwner()->GetVelocity();
			otherSkill->skillHit(this, GetOwner(), otherSkillData, locationOfCollision);
		}
		else
		{
			if (FVector(GetOwner()->GetActorLocation() - locationOfCollision).X < 0)
				kb.X = -kb.X;
		}

		getKnockedBack(kb);
	}

	if (otherSkillData.skillData.skillDamageCoefficient != 0.0f)
		SpawnDamageText(dmgTextActor, damageAfterDefense, dmgType, criticalHit, totalStats.MaxHP, damageCauser, SkillUseID);
}
bool UAttributeComponent::TakeDamage_Validate(UAttributeComponent * damageCauser, UObject * damageSource, int SkillUseID, EDmgType DmgType, FAbstractSkillData otherSkillData, FPlayerAttributes otherAttributes, FVector locationOfCollision, TSubclassOf<ADamageTextActor> dmgTextActor) { return true; }

void UAttributeComponent::Heal_Implementation(float HealAmount)
{
	if (!GetOwner()->HasAuthority())
		return;
	if (!Alive)
		return;
	HealAmount = FMath::CeilToInt(HealAmount);

	if (currHp + HealAmount > totalStats.MaxHP)
		HealAmount = totalStats.MaxHP - currHp;

	if (HealAmount > 0)
	{
		currHp += HealAmount;
		SpawnDamageText(damageTextActorStyle, HealAmount, EDmgType::DT_Heal, false, totalStats.MaxHP, this, -1);
	}
}
bool UAttributeComponent::Heal_Validate(float HealAmount)
{
	return true;
}

void UAttributeComponent::Die_Implementation()
{
	// play sounds
	currRespawnTime = MaxRespawnTime;
	Alive = false;
	currHp = 0;
	//if (HpBar != nullptr && HpBar->IsValidLowLevel())
	//{
	//	HpBar->Destroy();
	//}

}
void UAttributeComponent::TogglePVP_Implementation()
{
	//if (!GetOwner()->HasAuthority())
	//	return;

	bPvpEnabled = !bPvpEnabled;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, bPvpEnabled ? "PVPEnabled" : "PVPDisabled");

	//NetMulticastRPC_TogglePVP(bPvpEnabled);
}
bool UAttributeComponent::TogglePVP_Validate() { return true; }

void UAttributeComponent::SetEvade_Implementation(bool isEvade)
{
	bIsEvading = isEvade;
}
bool UAttributeComponent::SetEvade_Validate(bool isEvade) { return true; }
//void UAttributeComponent::NetMulticastRPC_TogglePVP_Implementation(bool pvp)
//{
//	bPvpEnabled = pvp;
//}
//bool UAttributeComponent::NetMulticastRPC_TogglePVP_Validate(bool pvp) { return true; }
//bool UAttributeComponent::Die_Validate()
//{
//	return true;
//}

void UAttributeComponent::Respawn_Implementation()
{
	if (!GetOwner()->HasAuthority())
		return;
	// play sounds
	Alive = true;
	currJumps = totalStats.MaxJumps;
	RecalculateTotalStats();
	Heal(totalStats.MaxHP);
	onRespawnDelegate.Broadcast();

}
bool UAttributeComponent::Respawn_Validate() { return true; }

void UAttributeComponent::getKnockedBack(FVector knockBack)
{
	if (!GetOwner()->HasAuthority())
		return;

	if (GetOwner()->GetClass()->IsChildOf(ACharacter::StaticClass()))
	{
		FVector vKnockBack = knockBack;
		float stanceValue = (1.0f - totalStats.Stance);
		stanceValue = stanceValue >= 0.0f ? stanceValue : 0.0f;
		vKnockBack *= stanceValue;

		if (vKnockBack.IsNearlyZero())
			return;

		((ACharacter*)GetOwner())->LaunchCharacter(vKnockBack, true, true);
	}
}
void UAttributeComponent::SpawnDamageText_Implementation(TSubclassOf<ADamageTextActor> damageTextActorStyle, float finalDmg, EDmgType DmgType, bool CriticalHit, int MaxHp, UAttributeComponent * DamageCauser, int DmgTextSpawnID)
{
	if (!damageTextActorStyle)
		return;
	//if (!DamageCauser)
	//	return;

	//for (int i = DamageTextArr.Num() - 1; i >= 0; --i)
	//{
	//	if (!DamageTextArr[i] || !DamageTextArr[i]->IsValidLowLevel())
	//		DamageTextArr.RemoveAt(i);
	//}

	bool firstSpawn = false;
	ADamageTextActor * DmgTextActor = nullptr;
	float Lifetime = 0.75f;
	FVector SpawnOffset = GetOwner()->GetActorLocation() + FVector(0, 0, 20);
	FTransform spawnTransform(GetOwner()->GetActorRotation(), SpawnOffset);

	if (DamageTextArr.Contains(DamageCauser))
	{
		for (size_t i = 0; i < DamageTextArr[DamageCauser].Num(); i++)
		{
			if (DmgTextSpawnID	== DamageTextArr[DamageCauser][i]->DmgTextSpawnID &&
				DmgType			== DamageTextArr[DamageCauser][i]->DmgType)
			{
				DmgTextActor = DamageTextArr[DamageCauser][i];
				break;
			}
		}
	}
	else
	{
		DamageTextArr.Add(DamageCauser);
	}


	if (DmgTextActor)
	{
		DmgTextActor->Damage = DmgTextActor->Damage + finalDmg;

		DamageTextArr[DamageCauser].Remove(DmgTextActor);
	}
	else
	{
		firstSpawn = true;
		DmgTextActor = (ADamageTextActor*)GetOwner()->GetWorld()->
			SpawnActorDeferred<ADamageTextActor>(
				damageTextActorStyle,
				spawnTransform,
				GetOwner());

		if (!DmgTextActor)
			return;

		DmgTextActor->Damage			= finalDmg;
		DmgTextActor->DmgType			= DmgType;
		DmgTextActor->DmgTextSpawnID	= DmgTextSpawnID;
		DmgTextActor->DamageCauser		= DamageCauser;

	}
	DamageTextArr[DamageCauser].Insert(DmgTextActor, 0);

	DmgTextActor->Lifetime			= Lifetime;
	DmgTextActor->RemainingLifetime = DmgTextActor->Lifetime;
	DmgTextActor->CriticalHit		= CriticalHit;
	DmgTextActor->HitMaxHP			= (MaxHp != 0 ? MaxHp : 1.0f);
	DmgTextActor->Scale				= DmgTextActor->Damage / DmgTextActor->HitMaxHP;

	if (firstSpawn)
		DmgTextActor->FinishSpawning(spawnTransform);

	DmgTextActor->SetActorTransform(spawnTransform);

	for (int i = 0; i < DamageTextArr[DamageCauser].Num(); i++)
	{
		if (DamageTextArr[DamageCauser][i]->DmgTextSpawnID == DmgTextSpawnID)
			DamageTextArr[DamageCauser][i]->SetActorLocation(SpawnOffset + FVector(0, 0, 10 * i));
	}
}

bool UAttributeComponent::canTarget(UAttributeComponent * other)
{
	if (other == nullptr)
		return false;

	if (!other->isAlive())
		return false;

	//if (isInvuln)
	//	return false;

	//if (isDodging)
	//	return false;

	if (this == other)
		return false;

	if (AllianceType == other->AllianceType && this->bPvpEnabled && other->bPvpEnabled)
		return true;

	if (AllianceType != other->AllianceType)
		return true;

	return false;
}
void UAttributeComponent::RecalculateTotalStats_Implementation()
{
	//if (!GetOwner()->HasAuthority())
	//	return;

	totalStats = baseStats;

	TStatusMap & tempMap = StatusMap;
	//statusMap::reverse_iterator StatusMapIter;
	//attribMap::reverse_iterator AttribIter;

	for (int i = 0; i < EquippedItems.Num(); i++)
	{
		totalStats = UStructHelperFunctions::AddAttributes(EquippedItems[i]->totalStats, totalStats);
	}

	for (auto StatusMapIter = tempMap.CreateIterator()/*.rbegin()*/; StatusMapIter/* != tempMap.rend()*/; ++StatusMapIter)
	{
		for (auto AttribIter = StatusMapIter->Value.CreateIterator()/*second.rbegin()*/; AttribIter /*!= StatusMapIter->second.rend()*/; ++AttribIter)
		{
			for (int Array_i = AttribIter->Value.Num() - 1; Array_i >= 0; --Array_i)
			{
				totalStats = UStructHelperFunctions::AddAttributes(
					UStructHelperFunctions::ScaleAttributes(
						AttribIter->Value[Array_i]->statModifier,
						AttribIter->Value[Array_i]->stacks.Num()),
					totalStats);
			}
		}
	}

	if (currHp > totalStats.MaxHP)
		currHp = totalStats.MaxHP;

	totalStats.Damage_Reduction += FMath::LogX(1.075f, totalStats.Defense + 1.0f) / 100.0f; // ~1400 max defense is 100% damage reduction;
	//totalStats.Damage_Reduction = totalStats.Damage_Reduction  < 0 ? 0 : totalStats.Damage_Reduction;

	totalStats.AttackSpeed *= (1 + totalStats.AttackSpeed_Multiplier);
	updateStatusBarStatuses(true);
}
bool UAttributeComponent::RecalculateTotalStats_Validate() { return true; }

ATest2DAlly * UAttributeComponent::spawnStatus(UAttributeComponent * appliedBy, FStatusApplicationData statusApplicationData)
{
	FActorSpawnParameters fSpawn;
	fSpawn.Owner = GetOwner();
	fSpawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ATest2DAlly * toSpawn = GetOwner()->GetWorld()->SpawnActorDeferred<ATest2DAlly>(
		statusApplicationData.statusClass,
		GetOwner()->GetTransform(),
		GetOwner(),
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	toSpawn->Init(appliedBy, this, statusApplicationData.statusLifetime);

	toSpawn->FinishSpawning(GetOwner()->GetTransform());

	StatusMap[statusApplicationData.statusClass][appliedBy].Add(toSpawn);

	return toSpawn;
}

TArray<ATest2DAlly *> UAttributeComponent::applyStatus(TArray<FStatusApplicationData> statusesToApply, UAttributeComponent * appliedBy)
{
	TArray<ATest2DAlly *> status;

	if (statusesToApply.Num() == 0)
		return status;

	if (!Alive)
		return status;

	for (int data_i = 0; data_i < statusesToApply.Num(); data_i++)
	{
		FStatusApplicationData &statusApplicationData = statusesToApply[data_i];
		ATest2DAlly * statusDefaults = statusApplicationData.statusClass.GetDefaultObject();

		if (!StatusMap.Contains(statusApplicationData.statusClass))
			StatusMap.Add(statusApplicationData.statusClass);

		if (!StatusMap[statusApplicationData.statusClass].Contains(appliedBy))
			StatusMap[statusApplicationData.statusClass].Add(appliedBy);

		TArray<ATest2DAlly *> &appliedByMapIndex = StatusMap[statusApplicationData.statusClass][appliedBy];

		int stacksToApply = (statusApplicationData.numStacks > statusDefaults->maxStacks) ? statusDefaults->maxStacks : statusApplicationData.numStacks;

		if (statusApplicationData.statusClass)
		{
			for (int numAllies_i = 0; numAllies_i < stacksToApply; numAllies_i++)
			{
				if (!statusDefaults->bStackInternal)
				{
					status.Add(spawnStatus(appliedBy, statusApplicationData));

					if (appliedByMapIndex.Num() > statusDefaults->maxStacks)
					{
						if (appliedByMapIndex[0])
							appliedByMapIndex[0]->Destroy();
					}
				}
				else
				{
					if (appliedByMapIndex.Num() == 0)
					{
						status.Add(spawnStatus(appliedBy, statusApplicationData));
					}
					else
					{
						appliedByMapIndex[0]->stacks.Add(statusApplicationData.statusLifetime);
						status.Add(appliedByMapIndex[0]);

						if (appliedByMapIndex[0]->stacks.Num() > statusDefaults->maxStacks)
							appliedByMapIndex[0]->stacks.RemoveAt(0, 1, true);
					}
				}
			}
			AppliedStatuses.AddUnique(statusApplicationData.statusClass);
		}
	}
	RecalculateTotalStats();
	return status;
}
void UAttributeComponent::removeStatus_Implementation(ATest2DAlly * statusToRemove)
{
	if (!statusToRemove)
		return;

	StatusMap[statusToRemove->GetClass()][statusToRemove->ownerAttribComp].Remove(statusToRemove);
	if (StatusMap[statusToRemove->GetClass()].Num() == 0/*.empty()*/ ||
		StatusMap[statusToRemove->GetClass()][statusToRemove->ownerAttribComp].Num() == 0)
		AppliedStatuses.Remove(statusToRemove->GetClass());

	RecalculateTotalStats();
}
bool UAttributeComponent::removeStatus_Validate(ATest2DAlly * statusToRemove) { return true; }


void UAttributeComponent::clearStatuses_Implementation()
{
	//statusMap::reverse_iterator StatusMapIter;
	//attribMap::reverse_iterator AttribIter;

	for (auto StatusMapIter = StatusMap.CreateIterator()/*.rbegin()*/; StatusMapIter/* != StatusMap.rend()*/; ++StatusMapIter)
	{
		for (auto AttribIter = StatusMapIter->Value.CreateIterator()/*second.rbegin()*/; AttribIter/* != StatusMapIter->second.rend()*/; ++AttribIter)
		{
			for (int Array_i = AttribIter->Value/*second*/.Num() - 1; Array_i >= 0; --Array_i)
			{
				AttribIter->Value[Array_i]->Destroy();
			}
		}
		StatusMapIter->Value.Empty();
	}
	StatusMap.Empty();
	AppliedStatuses.Empty();

	RecalculateTotalStats();
}
bool UAttributeComponent::clearStatuses_Validate() { return true; }

int UAttributeComponent::getTotalStacksFromStatus(TSubclassOf<ATest2DAlly> statusClass)
{
	int ret = 0;

	if (StatusMap.Contains(statusClass))
	{
		TAttribMap & tempMap = StatusMap[statusClass];
		//attribMap::iterator AttribIter;

		for (auto AttribIter = tempMap.CreateIterator()/*.begin()*/; AttribIter/* != tempMap.end()*/; ++AttribIter)
		{
			for (int Array_i = AttribIter->Value.Num() - 1; Array_i >= 0; --Array_i)
			{
				ret += AttribIter->Value[Array_i]->stacks.Num();
			}
		}
	}
	return ret;
}
void UAttributeComponent::updateStatusBar_RepNotify()
{
	onUpdateStatusBarDelegate.Broadcast();
}
void UAttributeComponent::incrSkillUseID_Implementation()
{
	++skillCounter;
}
bool UAttributeComponent::incrSkillUseID_Validate() { return true; }

void UAttributeComponent::updateStatusBarStatuses_Implementation(bool checkForShowOnStatusBar)
{
	if (!GetOwner()->HasAuthority())
		return;

	statusBarStatuses.Empty();

	int totalStacksFromClass = 0;

	TStatusMap & tempMap = StatusMap;

	for (int i = 0; i < AppliedStatuses.Num(); i++)
	{
		totalStacksFromClass = getTotalStacksFromStatus(AppliedStatuses[i]);
		if (totalStacksFromClass > 0)
		{
			statusBarStatuses.Add(FStatusApplicationData(AppliedStatuses[i], totalStacksFromClass));
		}
	}
	updateStatusBar_RepNotify();
}
bool UAttributeComponent::updateStatusBarStatuses_Validate(bool checkForShowOnStatusBar) { return true; }

UTexture2D * UAttributeComponent::getStatusIcon(TSubclassOf<ATest2DAlly> statusClass)
{
	return statusClass.GetDefaultObject()->StatusIcon;
}

void UAttributeComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAttributeComponent, baseStats);
	DOREPLIFETIME(UAttributeComponent, totalStats);
	DOREPLIFETIME(UAttributeComponent, currHp);
	DOREPLIFETIME(UAttributeComponent, currJumps);
	DOREPLIFETIME(UAttributeComponent, Alive);/*
	DOREPLIFETIME(UAttributeComponent, activeDebuffs);
	DOREPLIFETIME(UAttributeComponent, activeBuffs);*/

	//DOREPLIFETIME(UAttributeComponent, StatusMap);
	DOREPLIFETIME(UAttributeComponent, AppliedStatuses);
	DOREPLIFETIME(UAttributeComponent, bPvpEnabled);

	DOREPLIFETIME(UAttributeComponent, bIsEvading);
	DOREPLIFETIME(UAttributeComponent, bIsInvuln);

	DOREPLIFETIME(UAttributeComponent, statusBarStatuses);
	DOREPLIFETIME(UAttributeComponent, skillCounter);


	//DOREPLIFETIME(UAttributeComponent, totalSkillsUsed);


}