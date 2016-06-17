// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Test2DEnums.h"
#include "Status.h"
#include "PlayerAttributes.h"
#include "Item.generated.h"


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
		EItemType ItemType = EItemType::IT_ETC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		UTexture2D * ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		int MaxStack = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
		int CurrStack = 1;
};
