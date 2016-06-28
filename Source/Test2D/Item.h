// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Test2DEnums.h"
#include "Status.h"
#include "PlayerAttributes.h"
#include "Item.generated.h"

class UAttributeComponent;
class UInventoryComponent;

UCLASS()
class TEST2D_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		EItemType ItemType = EItemType::ETC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		UTexture2D * ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		FString ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		int MaxStack = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		int CurrStack = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		UAttributeComponent * AttributeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		UInventoryComponent * InventoryComponent;

	UFUNCTION(BlueprintCallable, Category = "Item")
		void UseItem();

	UFUNCTION(BlueprintNativeEvent, Category = "Item")
		void UseFunc();

	void Init_Implementation(UAttributeComponent * AttribComp, UInventoryComponent * InventoryComp);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
		void Init(UAttributeComponent * AttribComp, UInventoryComponent * InventoryComp);
};
