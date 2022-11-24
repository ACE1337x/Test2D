// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Test2DCharacter.h"

#include "Test2D.h"

#include "Kismet/KismetStringLibrary.h"
#include "Engine.h"

#include "PaperFlipbookComponent.h"
#include "Components/TextRenderComponent.h"

#include "AttributeComponent.h"
#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);
//////////////////////////////////////////////////////////////////////////
// ATest2DCharacter

ATest2DCharacter::ATest2DCharacter()
{
	// Setup the assets
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> RunningAnimationAsset;
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> IdleAnimationAsset;
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> FallingAnimationAsset;
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> WallSlidingAnimationAsset;
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> JumpingAnimationAsset;
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> DeathAnimationAsset;

		FConstructorStatics()
			: RunningAnimationAsset		(TEXT("/Game/Assets/Flipbooks/Walking.Walking"))
			, IdleAnimationAsset		(TEXT("/Game/Assets/Flipbooks/Idle.Idle"))
			, FallingAnimationAsset		(TEXT("/Game/Assets/Flipbooks/Falling.Falling"))
			, WallSlidingAnimationAsset	(TEXT("/Game/Assets/Flipbooks/Falling.Falling"))
			, JumpingAnimationAsset		(TEXT("/Game/Assets/Flipbooks/Jumping.Jumping"))
			, DeathAnimationAsset		(TEXT("/Game/Assets/Flipbooks/Dead.Dead"))

		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	RunningAnimation		= ConstructorStatics.RunningAnimationAsset.Get();
	IdleAnimation			= ConstructorStatics.IdleAnimationAsset.Get();
	FallingAnimation		= ConstructorStatics.FallingAnimationAsset.Get();
	WallSlidingAnimation	= ConstructorStatics.WallSlidingAnimationAsset.Get();
	JumpingAnimation		= ConstructorStatics.JumpingAnimationAsset.Get();
	DeathAnimation			= ConstructorStatics.DeathAnimationAsset.Get();

	GetSprite()->SetFlipbook(IdleAnimation);

	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw	= false;
	bUseControllerRotationRoll	= false;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(10.0f);
	GetCapsuleComponent()->SetCapsuleRadius(5.0f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->CanCharacterStepUpOn = ECB_Yes;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 1.0f;
	GetCharacterMovement()->AirControl = 0.3f;
	GetCharacterMovement()->JumpZVelocity = 350.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 200.0f;
	GetCharacterMovement()->MaxFlySpeed = 200.0f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;
	GetCharacterMovement()->MaxStepHeight = 8.0f;

	AttributeComponent	= CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
	InventoryComponent	= CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	SkillHandler		= CreateDefaultSubobject<USkillHandlerComponent>(TEXT("SkillHandler"));

	wallSlideCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("wallSlideCollision"));
	if (wallSlideCollision)
	{
		//wallSlideCollision->AttachTo(GetRootComponent());
		wallSlideCollision->SetupAttachment(GetRootComponent());
		wallSlideCollision->SetBoxExtent(FVector(8.0f, 5.0f, 5.0f), true);

		wallSlideCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		wallSlideCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	if (SceneComponent)
	{
		SceneComponent->ComponentTags.Add("HpBarSlot");
		SceneComponent->SetupAttachment(GetCapsuleComponent());
	}

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	AttributeComponent->SetIsReplicated(true);
	SkillHandler->SetIsReplicated(true);
	InventoryComponent->SetIsReplicated(true);

	bReplicates			= true;
	bReplicateMovement	= true;
	bAlwaysRelevant		= true;

}
void ATest2DCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComponent->Init(this);
	SkillHandler->Init(this);
	InventoryComponent->Init(this);

	Respawn();
}

void ATest2DCharacter::StatChange_RepNotify()
{
	onRecalculateTotalStats.Broadcast();
}
void ATest2DCharacter::RecalculateTotalStats_Implementation()
{
	int previousMaxJumps = totalStats.MaxJumps;

	totalStats = baseStats;
	AttributeComponent->RecalculateTotalStats();
	InventoryComponent->RecalculateTotalStats();

	totalStats = UStructHelperFunctions::AddAttributes(AttributeComponent->totalStats, totalStats);
	totalStats = UStructHelperFunctions::AddAttributes(InventoryComponent->totalStats, totalStats);

	totalStats.Damage_Reduction += FMath::LogX(1.075f, totalStats.Defense + 1.0f) / 100.0f; // ~1400 max defense is 100% damage reduction;
																							//totalStats.Damage_Reduction = totalStats.Damage_Reduction  < 0 ? 0 : totalStats.Damage_Reduction;
	totalStats.AttackSpeed *= (1 + totalStats.AttackSpeed_Multiplier);

	if (currHp > totalStats.MaxHP)
		currHp = totalStats.MaxHP;

	currJumps += totalStats.MaxJumps - previousMaxJumps;

	if (currJumps > totalStats.MaxJumps)
		currJumps = totalStats.MaxJumps;

	StatChange_RepNotify();
}
bool ATest2DCharacter::RecalculateTotalStats_Validate()
{
	return true;
}

void ATest2DCharacter::TryUseSkill_Implementation(ESkillSlotTypes skillSlotToUse)
{
	if (!wallSliding && SkillHandler && bIsAlive)
	{
		SkillHandler->tryUseSkillSlot(skillSlotToUse);
	}
}
bool ATest2DCharacter::TryUseSkill_Validate(ESkillSlotTypes skillSlotToUse)
{
	return true;
}
void ATest2DCharacter::BeginPlay()
{
	Super::BeginPlay();

	bIsAlive = true;

	//SceneComponent->SetRelativeLocation(FVector(0, 0, -(GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) + 7.0f));
}

//////////////////////////////////////////////////////////////////////////
// Animation

void ATest2DCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateCharacter(DeltaSeconds);

	UpdateAnimation();
	
	if (Role == ROLE_AutonomousProxy)
		UpdateRPC(actorRotation, lookDir, DesiredAnimation);

}


//////////////////////////////////////////////////////////////////////////
// Input

void ATest2DCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
}

void ATest2DCharacter::Jump_Implementation()
{
	if (!HasAuthority())
		return;
	if (!bIsAlive)
		return;
	wantsToJump = true;
}
bool ATest2DCharacter::Jump_Validate()
{
	return true;
}


void ATest2DCharacter::Landed_Implementation(const FHitResult &Hit)
{
	Super::Landed(Hit);
	if (!HasAuthority())
		return;

	currJumps = totalStats.MaxJumps;
}
bool ATest2DCharacter::Landed_Validate(const FHitResult &Hit)
{
	return true;
}

void ATest2DCharacter::MoveRight_Implementation(float Value)
{
	if (!HasAuthority())
		return;
	if (!bIsAlive)
		return;
	moveRightDir = Value;
}
bool ATest2DCharacter::MoveRight_Validate(float Value)
{
	return true;
}
void ATest2DCharacter::MoveUp_Implementation(float Value)
{
	if (!HasAuthority())
		return;
	if (!bIsAlive)
		return;
	moveUpDir = Value;
}
bool ATest2DCharacter::MoveUp_Validate(float Value)
{
	return true;
}

void ATest2DCharacter::UpdateCharacter/*_Implementation*/(float dt)
{
	if (!bIsAlive)
	{
		currRespawnTime -= dt;
		if (currRespawnTime <= 0)
		{
			Respawn();
		}
		return;
	}

	FVector LastMoveDir = FVector(SkillHandler->ToTargetDir.X, 0.f, 0.f);
	LastMoveDir.Normalize();

	if (!SkillHandler || !SkillHandler->getCurrSkill() || !SkillHandler->getCurrSkill()->isMovementSkill())
	{
		moveInput = !FMath::IsNearlyZero(moveRightDir);

		if (moveInput)
		{
			if (moveRightDir < 0.0f)
			{
				actorRotation	= FRotator(0.0, 180.0f, 0.0f);
				LastMoveDir		= FVector(-1.0f, 0.f, 0.f);
			}
			else if (moveRightDir > 0.0f)
			{
				actorRotation	= FRotator(0.0, 0.0f, 0.0f);
				LastMoveDir		= FVector(1.0f, 0.f, 0.f);
			}
			AddMovementInput(FVector(1.0f, 0.0f, 0.0f), moveRightDir);
		}
		if (!moveInput || SkillHandler->getCurrSkill() && SkillHandler->getCurrSkill()->isCrosshairAimed())
		{
			if (lookDir.X < 0.0f)
			{
				actorRotation	= FRotator(0.0, 180.0f, 0.0f);
			}

			else if (lookDir.X > 0.0f)
			{
				actorRotation	= FRotator(0.0, 0.0f, 0.0f);
			}
		}
		SkillHandler->LastMoveDir = LastMoveDir;
	}
	else
		moveInput = false;

	SkillHandler->ToTargetDir = lookDir;

	falling = (GetVelocity().Z < 0.0f);
	jumping = (GetVelocity().Z > 0.0f);

	wallSliding = false;

	FVector averageHitNormals(0, 0, 0);
	if (!FMath::IsNearlyZero(GetVelocity().Z) && !SkillHandler->getCurrSkill())
	{
		TArray<FHitResult> hitResults;
		FCollisionQueryParams queryParams;
		GetWorld()->SweepMultiByObjectType(
			hitResults,
			wallSlideCollision->GetComponentLocation(),
			wallSlideCollision->GetComponentLocation() + GetActorForwardVector(),
			wallSlideCollision->GetComponentRotation().Quaternion(),
			ECollisionChannel::ECC_WorldStatic,
			wallSlideCollision->GetCollisionShape(),
			queryParams);

		if (hitResults.Num() > 0)
		{
			for (size_t i = 0; i < hitResults.Num(); i++)
				averageHitNormals += hitResults[i].Normal;

			averageHitNormals.Normalize();
			wallSliding = true;

			if (falling)
			{
				float fallCoeff = FMath::GetMappedRangeValueClamped(FVector2D(1.0f, -1.0f), FVector2D(0.8f, 0.2f), moveUpDir);
				LaunchCharacter(FVector(0.0f, 0.0f, -GetVelocity().Z * fallCoeff), false, false);
			}

			if (averageHitNormals.X > 0.0f)
				actorRotation = FRotator(0.0, 180.0f, 0.0f);
			else if (averageHitNormals.X < 0.0f)
				actorRotation = FRotator(0.0, 0.0f, 0.0f);
		}
	}

	if (wantsToJump)
	{
		wantsToJump = false;
		if ((SkillHandler->getCurrSkill() && !SkillHandler->getCurrSkill()->isMovementSkill()) ||
			!SkillHandler->getCurrSkill())
		{
			if (wallSliding)
			{
				wallSliding = false;
				FVector launchDir = averageHitNormals;

				launchDir.Z = GetCharacterMovement()->JumpZVelocity;
				launchDir.X = GetCharacterMovement()->JumpZVelocity * 1.0f * ((launchDir.X > 0) ? 1.0f : -1.0f);
				LaunchCharacter(launchDir, true, true);

			}
			else if (currJumps > 0)
			{
				LaunchCharacter(FVector(0, 0, GetCharacterMovement()->JumpZVelocity), false, true);
				currJumps--;
			}
		}
	}

	SetActorRotation(actorRotation);
}
//bool ATest2DCharacter::UpdateCharacter_Validate() { return true; }

void ATest2DCharacter::UpdateAnimation()
{
	if (!HasAuthority())
		return;

	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

	// Are we moving or standing still?
	DesiredAnimation = IdleAnimation;
	if (moveInput)
		DesiredAnimation = RunningAnimation;
	if (falling)
		DesiredAnimation = FallingAnimation;
	if (wallSliding)
		DesiredAnimation = WallSlidingAnimation;
	if (jumping)
		DesiredAnimation = JumpingAnimation;
	if (!bIsAlive)
		DesiredAnimation = DeathAnimation;
	if (SkillHandler != nullptr && SkillHandler->IsValidLowLevel() && SkillHandler->getCurrSkillAnim())
		DesiredAnimation = SkillHandler->getCurrSkillAnim();

	if (GetSprite()->GetFlipbook() != DesiredAnimation)
		GetSprite()->SetFlipbook(DesiredAnimation);
}
void ATest2DCharacter::UpdateRPC_Implementation(FRotator ARot, FVector VLook, UPaperFlipbook * newAnimation)
{
	actorRotation = ARot;
	SetActorRotation(ARot);

	lookDir = VLook;
	SkillHandler->ToTargetDir = VLook;

	DesiredAnimation = newAnimation;
	GetSprite()->SetFlipbook(newAnimation);
}
bool ATest2DCharacter::UpdateRPC_Validate(FRotator ARot, FVector VLook, UPaperFlipbook * newAnimation)
{
	return true;
}

bool ATest2DCharacter::canTarget(ATest2DCharacter * other)
{
	if (other == nullptr)
		return false;

	if (!other->bIsAlive)
		return false;

	if (this == other)
		return false;

	if (AllianceType == other->AllianceType && this->bPvpEnabled && other->bPvpEnabled)
		return true;

	if (AllianceType != other->AllianceType)
		return true;

	return false;
}

void ATest2DCharacter::TakeDamageTest_Implementation(
	ATest2DCharacter * damageCauser,
	UObject * damageSource,
	int SkillUseID,
	EDmgType DmgType,
	FAbstractSkillData otherSkillData,
	FPlayerAttributes otherAttributes,
	FVector locationOfCollision,
	TSubclassOf<ADamageTextActor> dmgTextActor)
{
	if (!HasAuthority())
		return;

	if (!bIsAlive)
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
	}

	onTakeDamageDelegate.Broadcast(damageAfterDefense, damageBeforeDefense, damageCauser, dmgType);

	if (!bIsInvuln && !bIsEvading)
	{
		currHp -= damageAfterDefense;

		if (currHp <= 0 && bIsAlive)
		{
			Die();
		}

		if (locationOfCollision == FVector::ZeroVector)
			locationOfCollision = GetActorLocation();

		if (damageCauser)
		{
			damageCauser->Heal(damageAfterDefense * totalLifeSteal);
		}

		FVector kb = otherSkillData.skillData.knockBack;

		if (damageSource && damageSource->GetClass()->IsChildOf(UAbstractSkill::StaticClass()))
		{
			UAbstractSkill * otherSkill = (UAbstractSkill*)damageSource;

			if (FVector(GetActorLocation() - otherSkill->initialSkillUseLoc).X < 0)
				kb.X = -kb.X;

			if (otherSkillData.skillInteractionInfo.knockbackUseVelocity)
				kb += otherSkill->Test2DCharacter->GetVelocity();

			otherSkill->skillHit(this, otherSkillData, locationOfCollision);
		}
		else
		{
			if (FVector(GetActorLocation() - locationOfCollision).X < 0)
				kb.X = -kb.X;
		}

		getKnockedBack(kb);
	}

	if (otherSkillData.skillData.skillDamageCoefficient != 0.0f)
		SpawnDamageText(dmgTextActor, damageAfterDefense, dmgType, criticalHit, totalStats.MaxHP, damageCauser, SkillUseID);
}
bool ATest2DCharacter::TakeDamageTest_Validate(
	ATest2DCharacter * damageCauser,
	UObject * damageSource,
	int SkillUseID,
	EDmgType DmgType,
	FAbstractSkillData otherSkillData,
	FPlayerAttributes otherAttributes,
	FVector locationOfCollision,
	TSubclassOf<ADamageTextActor> dmgTextActor)
{
	return true;
}
void ATest2DCharacter::SpawnDamageText_Implementation(TSubclassOf<ADamageTextActor> damageTextActor, float finalDmg, EDmgType DmgType, bool CriticalHit, int MaxHp, ATest2DCharacter * DamageCauser, int DmgTextSpawnID)
{
	if (!damageTextActor)
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
	FVector SpawnOffset = GetActorLocation() + FVector(0, 0, 20);
	FTransform spawnTransform(GetActorRotation(), SpawnOffset);

	if (DamageTextArr.Contains(DamageCauser))
	{
		for (size_t i = 0; i < DamageTextArr[DamageCauser].Num(); i++)
		{
			if (DmgTextSpawnID == DamageTextArr[DamageCauser][i]->DmgTextSpawnID &&
				DmgType == DamageTextArr[DamageCauser][i]->DmgType)
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
		DmgTextActor = (ADamageTextActor*)GetWorld()->
			SpawnActorDeferred<ADamageTextActor>(
				damageTextActorStyle,
				spawnTransform,
				this);

		if (!DmgTextActor)
			return;

		DmgTextActor->Damage = finalDmg;
		DmgTextActor->DmgType = DmgType;
		DmgTextActor->DmgTextSpawnID = DmgTextSpawnID;
		DmgTextActor->DamageCauser = DamageCauser;

	}
	DamageTextArr[DamageCauser].Insert(DmgTextActor, 0);

	DmgTextActor->Lifetime = Lifetime;
	DmgTextActor->RemainingLifetime = DmgTextActor->Lifetime;
	DmgTextActor->CriticalHit = CriticalHit;
	DmgTextActor->HitMaxHP = (MaxHp != 0 ? MaxHp : 1.0f);
	DmgTextActor->Scale = DmgTextActor->Damage / DmgTextActor->HitMaxHP;

	if (firstSpawn)
		DmgTextActor->FinishSpawning(spawnTransform);

	DmgTextActor->SetActorTransform(spawnTransform);

	for (int i = 0; i < DamageTextArr[DamageCauser].Num(); i++)
	{
		if (DamageTextArr[DamageCauser][i]->DmgTextSpawnID == DmgTextSpawnID)
			DamageTextArr[DamageCauser][i]->SetActorLocation(SpawnOffset + FVector(0, 0, 10 * i));
	}
}

void ATest2DCharacter::getKnockedBack(FVector knockBack)
{
	if (!HasAuthority())
		return;

	FVector vKnockBack = knockBack;
	float stanceValue = (1.0f - totalStats.Stance);
	stanceValue = stanceValue >= 0.0f ? stanceValue : 0.0f;
	vKnockBack *= stanceValue;

	if (vKnockBack.IsNearlyZero())
		return;

	LaunchCharacter(vKnockBack, true, true);
}
void ATest2DCharacter::Heal_Implementation(float HealAmount)
{
	if (!HasAuthority())
		return;
	if (!bIsAlive)
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
bool ATest2DCharacter::Heal_Validate(float HealAmount)
{
	return true;
}

void ATest2DCharacter::Die_Implementation()
{
	// play sounds
	currRespawnTime = MaxRespawnTime;
	bIsAlive = false;
	currHp = 0;
	AttributeComponent->clearStatuses();
	//if (HpBar != nullptr && HpBar->IsValidLowLevel())
	//{
	//	HpBar->Destroy();
	//}

}
void ATest2DCharacter::TogglePVP_Implementation()
{
	//if (!GetOwner()->HasAuthority())
	//	return;

	bPvpEnabled = !bPvpEnabled;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, bPvpEnabled ? "PVPEnabled" : "PVPDisabled");

	//NetMulticastRPC_TogglePVP(bPvpEnabled);
}
bool ATest2DCharacter::TogglePVP_Validate() { return true; }

void ATest2DCharacter::SetEvade_Implementation(bool isEvade)
{
	bIsEvading = isEvade;
}
bool ATest2DCharacter::SetEvade_Validate(bool isEvade) { return true; }
//void UAttributeComponent::NetMulticastRPC_TogglePVP_Implementation(bool pvp)
//{
//	bPvpEnabled = pvp;
//}
//bool UAttributeComponent::NetMulticastRPC_TogglePVP_Validate(bool pvp) { return true; }
//bool UAttributeComponent::Die_Validate()
//{
//	return true;
//}

void ATest2DCharacter::Respawn_Implementation()
{
	if (!HasAuthority())
		return;
	// play sounds
	bIsAlive = true;
	currJumps = totalStats.MaxJumps;
	RecalculateTotalStats();
	Heal(totalStats.MaxHP);
	onRespawnDelegate.Broadcast();

}
bool ATest2DCharacter::Respawn_Validate() { return true; }


void ATest2DCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATest2DCharacter, SkillHandler);
	DOREPLIFETIME(ATest2DCharacter, AttributeComponent);
	DOREPLIFETIME(ATest2DCharacter, InventoryComponent);

	DOREPLIFETIME(ATest2DCharacter, actorRotation);
	DOREPLIFETIME(ATest2DCharacter, moveInput);
	DOREPLIFETIME(ATest2DCharacter, falling);
	DOREPLIFETIME(ATest2DCharacter, wallSliding);
	DOREPLIFETIME(ATest2DCharacter, jumping);
	DOREPLIFETIME(ATest2DCharacter, wantsToJump);
	DOREPLIFETIME(ATest2DCharacter, moveRightDir);
	DOREPLIFETIME(ATest2DCharacter, moveUpDir);
	DOREPLIFETIME(ATest2DCharacter, lookDir);
	DOREPLIFETIME(ATest2DCharacter, DesiredAnimation);

	DOREPLIFETIME(ATest2DCharacter, baseStats);
	DOREPLIFETIME(ATest2DCharacter, totalStats);
	DOREPLIFETIME(ATest2DCharacter, currHp);
	DOREPLIFETIME(ATest2DCharacter, currJumps);
	DOREPLIFETIME(ATest2DCharacter, bIsAlive);

	DOREPLIFETIME(ATest2DCharacter, bPvpEnabled);

	DOREPLIFETIME(ATest2DCharacter, bIsEvading);
	DOREPLIFETIME(ATest2DCharacter, bIsInvuln);
}