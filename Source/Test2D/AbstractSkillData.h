// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Test2DEnums.h"
#include "AbstractSkillData.generated.h"
/**
 *
 */

USTRUCT(BlueprintType) // for blueprint to show up as variable
struct FSkillCollisionData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillCollisionData")
		FVector  skillExtents = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillCollisionData")
		FVector  nearExtents = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillCollisionData")
		FRotator  orientation = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillCollisionData")
		FVector  localOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillCollisionData")
		ESkillCollisionType skillCollisionType = ESkillCollisionType::SC_Box;
};
USTRUCT(BlueprintType) // for blueprint to show up as variable
struct FSkillInteractionInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillInteractionInfo")
		bool isCrosshairAimed = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillInteractionInfo")
		bool canCancelSkills = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillInteractionInfo")
		bool canBeCancelled = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillInteractionInfo")
		bool movementSkill = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillInteractionInfo")
		bool knockbackUseVelocity = 0;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillInteractionInfo")
	//	bool knockbackUseVectorFromLocation = 0;
};
USTRUCT(BlueprintType) // for blueprint to show up as variable
struct FSkillUpdateData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillUpdateData")
		float skillWindUp = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillUpdateData")
		float afterCast = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillUpdateData")
		float timeBetweenHits = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillUpdateData")
		float skillCooldown = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillUpdateData")
		float skillInterruptCooldown = 0;

};
USTRUCT(BlueprintType) // for blueprint to show up as variable
struct FSkillData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillData")
		float skillDamageCoefficient = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillData")
		FVector  knockBack = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillData")
		float skillIncreasedCritChance = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillData")
		float skillIncreasedCritDamage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillData")
		float skillIncreasedLifesteal = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillData")
		int maxNumTargets = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillData")
		int numHits = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillData")
		int initiativeCost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillData")
		int initiativeRecoverOnHit = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSkillData	")
		bool isEvadable = 1;
};

USTRUCT(BlueprintType) // for blueprint to show up as variable
struct FAbstractSkillData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Collision")
		FSkillCollisionData skillCollisionData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Interaction")
		FSkillInteractionInfo skillInteractionInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Update")
		FSkillUpdateData skillUpdateData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Data")
		FSkillData skillData;
};