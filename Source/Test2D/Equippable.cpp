// Fill out your copyright notice in the Description page of Project Settings.

#include "Test2D.h"
#include "Equippable.h"
#include "InventoryComponent.h"

AEquippable::AEquippable()
{
	ItemType = EItemType::EQUIPPABLE;
}

void AEquippable::BeginPlay()
{
}

void AEquippable::Tick(float DeltaSeconds)
{
}

void AEquippable::UseFunc_Implementation()
{
	if (InventoryComponent)
		InventoryComponent->EquipItem(this);
}

void AEquippable::Init_Implementation(UAttributeComponent * AttribComp, UInventoryComponent * InventoryComp)
{
	Super::Init_Implementation(AttribComp, InventoryComp);

	totalStats = baseStats;
}
