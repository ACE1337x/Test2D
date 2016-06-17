// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "Item.h"
#include "InventoryComponent.generated.h"


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

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void Init();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int Currency = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		int MaxInventorySize = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		TArray<AItem*> InventoryItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		TArray<AItem*> EquippedItems;
};
