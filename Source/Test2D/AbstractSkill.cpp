// Fill out your copyright notice in the Description page of Project Settings.

#include "AbstractSkill.h"

#include "Test2D.h"
#include "Test2DCharacter.h"

#include "AttributeComponent.h"
#include "DrawDebugHelpers.h"

#include "BaseProjectile.h"
#include "PaperFlipbook.h"
#include "Net/UnrealNetwork.h"


void UAbstractSkill::Init(ATest2DCharacter * _character, AActor * _owner)
{
	owner = _owner;
	Test2DCharacter = _character;
	instigator = _character;

	if (skillData.skillUpdateData.skillInterruptCooldown == 0.0f)
		skillData.skillUpdateData.skillInterruptCooldown = skillData.skillUpdateData.skillCooldown;

	if (ActiveSkillPassives.Num())
		Destroy();

	if (skillName.IsEmpty())
		skillName = this->GetClass()->GetName();

	if (skillDescription.IsEmpty())
		skillDescription = this->GetClass()->GetName() + "_Description";

	InitSkillDescription();
	//attributeComponent

	ActiveSkillPassives = Test2DCharacter->AttributeComponent->applyStatus(SkillPassiveStatus, Test2DCharacter);

	setSkillHitBox();
}
void UAbstractSkill::Destroy()
{
	for (int passive_i = 0; passive_i < ActiveSkillPassives.Num(); passive_i++)
	{
		if (ActiveSkillPassives[passive_i])
			ActiveSkillPassives[passive_i]->Destroy();
	}

	ActiveSkillPassives.Empty();
}

void UAbstractSkill::SkillCompleted_Implementation()
{
	currCooldown = skillData.skillUpdateData.skillCooldown;
	bSkillComplete = true;
	bSkillUpdating = false;
	skillMotionType = ESkillMotion::SM_END;
	SkillMovement(skillMotionType, 0, FVector::ZeroVector, FVector::ZeroVector);
}

void UAbstractSkill::SkillMovement_Implementation(ESkillMotion update, float dt, FVector ToTargetDir, FVector LastMoveDir)
{
}
void UAbstractSkill::MulticastRPCFunc_spawnHitParticles_Implementation(FVector hitLocation)
{
	if (HitEffectParticle != nullptr)
	{
		UParticleSystemComponent * psys = UGameplayStatics::SpawnEmitterAtLocation(owner->GetWorld(), HitEffectParticle, hitLocation);
		psys->Activate();
	}
}

void UAbstractSkill::skillHit_Implementation(ATest2DCharacter * hitCharacter, FAbstractSkillData otherSkillData, FVector hitLocation)
{

	if (!owner || !owner->HasAuthority())
		return;

	MulticastRPCFunc_spawnHitParticles(hitLocation);

	//FVector kb = otherSkillData.skillData.knockBack;

	//if (FVector(hitObj->GetActorLocation() - initialSkillUseLoc).X < 0)
	//	kb.X = -kb.X;
	//if (otherSkillData.skillInteractionInfo.knockbackUseVelocity)
	//	kb += owner->GetVelocity();

	//hitAttribComp->getKnockedBack(kb);

	// apply on hits
	Test2DCharacter->AttributeComponent->applyStatus(Test2DCharacter->SelfStatusApplication_OnHit, Test2DCharacter);
	Test2DCharacter->AttributeComponent->applyStatus(SelfStatusApplication_OnHit, Test2DCharacter);

	hitCharacter->AttributeComponent->applyStatus(Test2DCharacter->EnemyStatusApplication_OnHit, Test2DCharacter);
	hitCharacter->AttributeComponent->applyStatus(EnemyStatusApplication_OnHit, Test2DCharacter);

	// do some cool particle effects

	//onSkillHitDelegate.Broadcast(hitAttribComp);
	Test2DCharacter->onSkillHitDelegate.Broadcast(hitCharacter, skillUseID);
}
bool UAbstractSkill::skillHit_Validate(ATest2DCharacter * hitCharacter, FAbstractSkillData otherSkillData, FVector hitLocation)
{
	return true;
}


void UAbstractSkill::setSkillHitBox() {
	FVector skillExtents = skillData.skillCollisionData.skillExtents;
	FVector nearExtents = skillData.skillCollisionData.nearExtents;

	switch (skillData.skillCollisionData.skillCollisionType)
	{
	case ESkillCollisionType::SC_Box: //box
	{
		hitBox = FCollisionShape::MakeBox(skillExtents * 0.5f);
		break;
	}
	case ESkillCollisionType::SC_Circle: // sphere
	{
		hitBox = FCollisionShape::MakeSphere(skillExtents.X * 0.5f);
		break;
	}
	}
}

void UAbstractSkill::update_Implementation(float dt, FVector ToTargetDir, FVector LastMoveDir)
{
	currSkillDuration += dt;
	delay -= dt;

	if ((owner && !owner->HasAuthority()) || !owner)
	{
		return;
	}

	if (!owner->IsValidLowLevel() || !Test2DCharacter)
	{
		bSkillUpdating = false;
		return;
	}

	if (bWantsToInterrupt)
	{
		SkillInterrupted();
		return;
	}

	SkillMovement(skillMotionType, dt, ToTargetDir, LastMoveDir);
	skillMotionType = ESkillMotion::SM_UPDATE;

	if (delay <= 0.0f)
	{
		delay = 0.0f;
		if (currNumHits == 0)
		{
			Test2DCharacter->AttributeComponent->applyStatus(SelfStatusApplication_OnStart, Test2DCharacter);
		}
		if (currNumHits < totalHits)
		{

			possibleTargets.Empty();

			FVector useSkillDir = owner->GetActorForwardVector();
			if (skillData.skillInteractionInfo.isCrosshairAimed)
			{
				useSkillDir = ToTargetDir;
			}
			FRotator temp = useSkillDir.ToOrientationRotator() + currOrientation;
			useSkillDir = temp.Vector();

			//if (owner->GetClass()->IsChildOf(ATest2DCharacter::StaticClass()))
			//{
			//	useSkillDir = ((ATest2DCharacter*)owner)->lookDir;
			//}

			FVector projectileLocation = owner->GetActorLocation() +
				(owner->GetActorForwardVector() * skillData.skillCollisionData.localOffset) +
				(owner->GetActorUpVector()* skillData.skillCollisionData.localOffset) +
				(owner->GetActorRightVector() * skillData.skillCollisionData.localOffset);


			FVector center = projectileLocation +
				((useSkillDir)* skillData.skillCollisionData.skillExtents.X * 0.5f);

			// spawn projectiles
			spawnProjectiles(projectileLocation, useSkillDir);

			allocateTargets(possibleTargets, center, useSkillDir);

			int maxNumTargets = skillData.skillData.maxNumTargets;

			int possibleTargetNum = possibleTargets.Num() < maxNumTargets ? possibleTargets.Num() : maxNumTargets;

			for (int i = 0; i < possibleTargetNum; i++)
			{
				ATest2DCharacter * currTarget = (ATest2DCharacter*)possibleTargets[i].GetActor();

				if (currTarget != nullptr && currTarget->IsValidLowLevel() && currTarget != Test2DCharacter)
				{

					currTarget->TakeDamageTest(
						Test2DCharacter,
						this,
						skillUseID,
						EDmgType::DT_Skill,
						skillData,
						Test2DCharacter->totalStats,
						possibleTargets[i].ImpactPoint,
						Test2DCharacter->damageTextActorStyle
						);
				}
			}
			delay = skillData.skillUpdateData.timeBetweenHits;
			if (skillData.skillInteractionInfo.isNumHitsModifiable)
			{
				delay /= ((float)totalHits / (float)skillData.skillData.numHits);
				delay /= Test2DCharacter->totalStats.AttackSpeed;
			}

			currNumHits++;
			if (currNumHits == totalHits)
				delay = skillData.skillUpdateData.afterCast / Test2DCharacter->totalStats.AttackSpeed;
		}
		else
		{
			SkillCompleted();
			return;
		}
	}
}
bool UAbstractSkill::update_Validate(float dt, FVector ToTargetDir, FVector LastMoveDir)
{
	return true;
}


void UAbstractSkill::decrementCooldown_Implementation(float dt)
{
	if (!owner || !owner->HasAuthority())
		return;

	if (currCooldown > 0.0f)
	{
		currCooldown -= dt;
	}
	else
		currCooldown = 0.0f;
}
void UAbstractSkill::spawnProjectiles_Implementation(FVector location, FVector useSkillDir)
{
	if ((owner && !owner->HasAuthority()) || !owner)
		return;

	for (int numProj_i = 0; numProj_i < projectilesPerHit.Num(); numProj_i++)
	{
		if (!projectilesPerHit[numProj_i].projectileClass)
			continue;

		if (projectilesPerHit[numProj_i].currHitToSpawn != -1 && currNumHits % skillData.skillData.numHits != projectilesPerHit[numProj_i].currHitToSpawn)
			continue;

		FTransform transform;
		transform.SetLocation(location);
		FRotator temp;
		temp = useSkillDir.ToOrientationRotator() + projectilesPerHit[numProj_i].projectileRotation;
		transform.SetRotation(temp.Quaternion());

		FActorSpawnParameters spawnParams;
		spawnParams.Owner = owner;
		ABaseProjectile * projectile = owner->GetWorld()->SpawnActorDeferred<ABaseProjectile>(projectilesPerHit[numProj_i].projectileClass, transform, owner, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		projectile->skill = this;
		projectile->numPierces = skillData.skillData.maxNumTargets - 1;

		projectiles.Add(projectile);

		projectile->FinishSpawning(transform);
	}
}
bool UAbstractSkill::spawnProjectiles_Validate(FVector location, FVector useSkillDir)
{
	return true;
}
void UAbstractSkill::allocateTargets(TArray<FHitResult> &hitObjects, FVector center, FVector useSkillDir)
{
	if (hitBox.GetExtent().IsNearlyZero())
		return;

	FHitResult outHit;
	FCollisionObjectQueryParams ObjectQueryParams;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(owner);

	QueryParams.TraceTag = FName("Tag");
	owner->GetWorld()->DebugDrawTraceTag = FName("Tag");

	//owner->GetWorld()->SweepMultiByChannel(
	owner->GetWorld()->SweepMultiByObjectType(
		hitObjects,
		center,
		center + useSkillDir,
		useSkillDir.ToOrientationRotator().Quaternion(),
		//owner->GetActorRotation().Quaternion(),
		ECollisionChannel::ECC_Pawn
		//ObjectQueryParams
		,
		hitBox,
		QueryParams
		);

	for (int i = hitObjects.Num() - 1; i >= 0; --i)
	{
		if (Test2DCharacter == hitObjects[i].GetActor() || 
			!hitObjects[i].GetActor()->GetClass()->IsChildOf(ATest2DCharacter::StaticClass()) || 
			!Test2DCharacter->canTarget(((ATest2DCharacter*)hitObjects[i].GetActor())))
		{
			hitObjects.RemoveAt(i, 1, false);
			continue;
		}
	}
}

void UAbstractSkill::SkillInterrupted_Implementation()
{
	currCooldown = skillData.skillUpdateData.skillInterruptCooldown;
	bWantsToInterrupt = false;
	bSkillUpdating = false;
	skillMotionType = ESkillMotion::SM_END;
	SkillMovement(skillMotionType, 0, FVector::ZeroVector, FVector::ZeroVector);
}

void UAbstractSkill::activateSkill(int _SkillUseID)
{
	if (Test2DCharacter)
	{
		skillUseID = _SkillUseID;

		totalHits = GetTotalHits();

		if (skillData.skillData.numHits)
			totalSkillDuration = ((totalHits * (skillData.skillUpdateData.timeBetweenHits) / (totalHits / skillData.skillData.numHits)) + skillData.skillUpdateData.skillWindUp + skillData.skillUpdateData.afterCast) / Test2DCharacter->totalStats.AttackSpeed;
		else
			totalSkillDuration = skillData.skillUpdateData.skillWindUp + skillData.skillUpdateData.afterCast / Test2DCharacter->totalStats.AttackSpeed;

		for (int i = 0; i < SelfStatusApplication_OnStart.Num(); i++)
		{
			if (SelfStatusApplication_OnStart[i].matchSkillDuration)
				SelfStatusApplication_OnStart[i].statusLifetime = totalSkillDuration;
		}

		currSkillDuration = 0;
		delay = skillData.skillUpdateData.skillWindUp;
		currNumHits = 0;
		currOrientation = skillData.skillCollisionData.orientation;
		skillMotionType = ESkillMotion::SM_START;

		bSkillComplete = false;
		bSkillUpdating = true;
		initialSkillUseLoc = owner->GetActorLocation();

		projectiles.Empty();
	}
}

//private class distanceComp implements Comparator<Entity> {
//	public int compare(Entity t, Entity t1) {
//		return t.getNode().getWorldTranslation().distanceSquared(owner.getNode().getWorldTranslation())
//		> t1.getNode().getWorldTranslation().distanceSquared(owner.getNode().getWorldTranslation()) ? 1 : -1;
//	}
//}


//public List<StatusApplicationData> getDebuffsToApplyOnHit() {
//	return skillData.debuffsToApplyOnHit;
//}
//
//public List<StatusApplicationData> getBuffsToApplyOnHit() {
//	return skillData.buffsToApplyOnHit;
//}
//
//public List<StatusApplicationData> getBuffsToApplyOnStart() {
//	return skillData.buffsToApplyOnStart;
//}


bool UAbstractSkill::canSkillBeCancelled() {
	return skillData.skillInteractionInfo.canBeCancelled;
}

bool UAbstractSkill::canCancelSkills() {
	return skillData.skillInteractionInfo.canCancelSkills;
}

bool UAbstractSkill::canBeUsed() {
	return (currCooldown <= 0 && useReq());
}

int UAbstractSkill::GetTotalHits()
{
	if (skillData.skillData.numHits > 0 && skillData.skillInteractionInfo.isNumHitsModifiable)
		return ((skillData.skillData.numHits + Test2DCharacter->totalStats.AdditionalHits) * (1 + Test2DCharacter->totalStats.HitMultiplier));

	return skillData.skillData.numHits;
}

void UAbstractSkill::InitSkillDescription_Implementation()
{
}

bool UAbstractSkill::useReq_Implementation() {
	//return owner.getCurrInitiative() >= skillData.getInt("initiativeCost");
	//custom requirement for specific skills, can be overriden
	return true;
}

void UAbstractSkill::cancelSkill()
{
	bWantsToInterrupt = true;
	update(0, FVector(0, 0, 0), FVector(0, 0, 0));
}

FAbstractSkillData UAbstractSkill::getSkillData() {
	return skillData;
}


FVector UAbstractSkill::getKnockbackMod() {
	return skillData.skillData.knockBack;
}

bool UAbstractSkill::isMovementSkill() {
	//return skillData.getBool("movementSkill");
	return skillData.skillInteractionInfo.movementSkill;
}
bool UAbstractSkill::isCrosshairAimed() {
	//return skillData.getBool("movementSkill");
	return skillData.skillInteractionInfo.isCrosshairAimed;
}

void UAbstractSkill::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAbstractSkill, delay);
	DOREPLIFETIME(UAbstractSkill, currCooldown);
	DOREPLIFETIME(UAbstractSkill, currNumHits);
	DOREPLIFETIME(UAbstractSkill, totalHits);

	DOREPLIFETIME(UAbstractSkill, bSkillComplete);
	DOREPLIFETIME(UAbstractSkill, bSkillUpdating);
	DOREPLIFETIME(UAbstractSkill, skillAnimation);

	DOREPLIFETIME(UAbstractSkill, Test2DCharacter);

	DOREPLIFETIME(UAbstractSkill, skillName);
	DOREPLIFETIME(UAbstractSkill, skillDescription);




}