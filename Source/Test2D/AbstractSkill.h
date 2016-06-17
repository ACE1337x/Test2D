// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "AbstractSkillData.h"
//#include "AbstractStatus.h"
#include "BaseProjectile.h"
#include "Status.h"
#include "AbstractSkill.generated.h"

/**
 *
 */
class UAttributeComponent;
class UTexture2D;
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSkillHitDelegate, UAttributeComponent *, Hit);

UCLASS(Blueprintable, BlueprintType)
class TEST2D_API UAbstractSkill : public UObject
{
	GENERATED_BODY()

protected:
	int skillID = -1;

	TArray<FHitResult> possibleTargets;
	FCollisionShape hitBox;

	void setSkillHitBox();

	void allocateTargets(TArray<FHitResult> &hitObjects, FVector center, FVector useSkillDir);

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "Skill")
		void spawnProjectiles(FVector location, FVector useSkillDir);

	FRotator currOrientation;

private:

	UPROPERTY(Replicated)
		float delay = 0.0f;
	UPROPERTY(Replicated)
		int currNumHits = 0;

	UPROPERTY(Replicated)
		bool bSkillComplete = false;
public:

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Skill")
		int totalHits = 0;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Skill")
		float currCooldown = 0.0f;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Skill")
		bool bSkillUpdating = false;

	virtual bool IsSupportedForNetworking() const override { return true; };

	void Init(AActor * owner, UAttributeComponent * attributeComponent, AActor * instigator = nullptr);
	void Destroy();


	UPROPERTY(BlueprintReadWrite, Category = "Skill")
		AActor * owner;

	AActor * GetOwner() { return owner; }
	void SetOwner(AActor * newOwner) { owner = newOwner; }

	UPROPERTY(BlueprintReadWrite, Category = "Skill")
		AActor * instigator;
	AActor * GetInstigator() { return instigator; }
	void SetInstigator(AActor * newInstigator) { instigator = newInstigator; }

	UPROPERTY(Replicated, BlueprintReadOnly, Category = EntityAttributes)
		UAttributeComponent * attributeComponent;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
		ESkillMotion skillMotionType;

	UPROPERTY(BlueprintReadOnly, Category = "Skill")
		bool bWantsToInterrupt = false;

	FVector getKnockbackMod();
	UPROPERTY(BlueprintReadOnly, Category = Movement)
		FVector initialSkillUseLoc;
	UPROPERTY(BlueprintReadOnly, Category = "Skill")
		TArray<UObject*> projectiles;

	UFUNCTION(BlueprintCallable, Category = "Skill")
		int GetTotalHits();

	void InitSkillDescription_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Skill")
		void InitSkillDescription();

	bool useReq_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Skill")
		bool useReq();

	void SkillMovement_Implementation(ESkillMotion update, float dt, FVector ToTargetDir, FVector LastMoveDir);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Skill")
		void SkillMovement(ESkillMotion update, float dt, FVector ToTargetDir, FVector LastMoveDir);

	UPROPERTY(BlueprintReadOnly, Category = "Skill")
		float currSkillDuration = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Skill")
		float totalSkillDuration = 0;

	bool isSkillUpdating() { return bSkillUpdating; }
	bool isSkillComplete() { return bSkillComplete; }

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Skill")
		void SkillCompleted();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Skill")
		void SkillInterrupted();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Skill")
		void decrementCooldown(float dt);

	void activateSkill();
	void cancelSkill();

	float getCurrCooldown() { return currCooldown; }

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "Skill")
		void update(float dt, FVector ToTargetDir, FVector LastMoveDir);

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "Skill")
		void skillHit(UAttributeComponent * hitAttribComp, AActor * hitObj, FAbstractSkillData otherSkillData, FVector hitLocation);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Skill")
		void MulticastRPCFunc_spawnHitParticles(FVector hitLocation);

	bool isMovementSkill();
	bool isCrosshairAimed();

	bool canSkillBeCancelled();
	bool canCancelSkills();

	UFUNCTION(BlueprintCallable, Category = "Skill")
		bool canBeUsed();

	FAbstractSkillData getSkillData();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
		UTexture2D * SkillIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
		UParticleSystem * HitEffectParticle;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Skill Animation")
	class UPaperFlipbook* skillAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
		FAbstractSkillData skillData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill_Projectiles")
		TArray<FProjectileSpawnData> projectilesPerHit;

	UPROPERTY(BlueprintReadOnly, Category = "Skill")
		TArray<ATest2DAlly*> ActiveSkillPassives;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill_Statuses")
		TArray<FStatusApplicationData> SkillPassiveStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill_Statuses")
		TArray<FStatusApplicationData> SelfStatusApplication_OnStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill_Statuses")
		TArray<FStatusApplicationData> SelfStatusApplication_OnHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill_Statuses")
		TArray<FStatusApplicationData> EnemyStatusApplication_OnHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill_Combo")
		TSubclassOf<UAbstractSkill> nextSkillInChain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill_Combo")
		TSubclassOf<UAbstractSkill> firstSkillInChain;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Skill")
		FString skillName;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Skill")
		FString skillDescription;

	int skillUseID = 0;
};
