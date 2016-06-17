// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Item.h"
#include "Equippable.generated.h"

/**
 *
 */
UCLASS()
class TEST2D_API AEquippable : public AItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEquippable();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		EEquipType EquipType = EEquipType::ET_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		EWeaponType WeaponType = EWeaponType::WT_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item_Stats")
		FPlayerAttributes baseStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item_Stats")
		FPlayerAttributes totalStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item_Stats")
		TArray<FStatusApplicationData> ItemEquipPassive;
};
