// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlayerAttributes.generated.h"

/**
 *
 */
//UCLASS(BlueprintType)
USTRUCT(BlueprintType)
/*class*/ struct TEST2D_API /*U*/FPlayerAttributes /*: public UObject*/
{
	GENERATED_USTRUCT_BODY()
	//GENERATED_BODY()
//public:
	//UPlayerAttributes();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EntityAttributes)
		int Damage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EntityAttributes)
		int MaxHP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EntityAttributes)
		float HpRegeneration = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EntityAttributes)
		int MaxJumps = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EntityAttributes)
		int Defense = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EntityAttributes)
		float Damage_Reduction = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EntityAttributes)
		float Defense_Penetration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EntityAttributes)
		float Critical_Chance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EntityAttributes)
		float Critical_Damage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EntityAttributes)
		float LifeSteal = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EntityAttributes)
		float AttackSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EntityAttributes)
		float AttackSpeed_Multiplier = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EntityAttributes)
		float AdditionalHits = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EntityAttributes)
		float HitMultiplier = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EntityAttributes)
		float Stance = 0.0f;
};

UCLASS()
class TEST2D_API UStructHelperFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Helper Functions|EntityAttributes")
		static FPlayerAttributes AddAttributes(FPlayerAttributes lhs, FPlayerAttributes rhs);

	UFUNCTION(BlueprintPure, Category = "Helper Functions|EntityAttributes")
		static FPlayerAttributes SubtractAttributes(FPlayerAttributes lhs, FPlayerAttributes rhs);

	UFUNCTION(BlueprintPure, Category = "Helper Functions|EntityAttributes")
		static FPlayerAttributes ScaleAttributes(FPlayerAttributes lhs, float rhs);

};
