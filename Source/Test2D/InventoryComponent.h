// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "Equippable.h"
#include "PlayerAttributes.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryEventDelegate);

USTRUCT(BlueprintType) // for blueprint to show up as variable
struct FItemLoadData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		TSubclassOf<AItem> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FPlayerAttributes ItemStatModifiers;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEST2D_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = Attributes)
		void RecalculateTotalStats();

	class ATest2DCharacter * Test2DCharacter;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void Init(ATest2DCharacter * _Test2DCharacter);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int Currency = 0;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int MaxInventorySize = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		TArray<FItemLoadData> InventoryLoadData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		TArray<FItemLoadData> EquippedLoadData;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Inventory")
		int SelectedIndex = -1;

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "Inventory")
		void SwapItems(int LHS, int RHS);

	UPROPERTY(BlueprintAssignable, Category = "Test")
		FInventoryEventDelegate onUpdateInventoryDelegate;

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "Inventory")
		void EquipItem(AEquippable * Equip);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		TArray<AItem*> InventoryItems;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "EquippedItems")
		TArray<AEquippable*>EquippedItems;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Attributes)
		FPlayerAttributes totalStats;
};

