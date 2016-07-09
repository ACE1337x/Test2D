// Fill out your copyright notice in the Description page of Project Settings.

#include "Test2D.h"
#include "Item.h"
#include "AttributeComponent.h"
#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AItem::AItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem::UseItem_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, HasAuthority() ? "Server" : "Client");
	UseFunc();
}
bool AItem::UseItem_Validate()
{
	return true;
}

void AItem::UseFunc_Implementation()
{
	if (ItemType == EItemType::USABLE)
	{
		CurrStack -= 1;
		if (CurrStack <= 0)
		{
			Destroy();
		}
	}
}

void AItem::Init_Implementation(UInventoryComponent * InventoryComp)
{
	//if (InventoryComp->GetOwner()->HasAuthority())
	//{
		InventoryComponent = InventoryComp;/*
		SetOwner(InventoryComponent->GetOwner());
	}*/
}

void AItem::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	DOREPLIFETIME(AItem, InventoryComponent);
	DOREPLIFETIME(AItem, CurrStack);
	DOREPLIFETIME(AItem, MaxStack);
}