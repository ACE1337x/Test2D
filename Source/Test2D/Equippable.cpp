// Fill out your copyright notice in the Description page of Project Settings.

#include "Test2D.h"
#include "Equippable.h"
#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"

AEquippable::AEquippable()
{
	ItemType = EItemType::EQUIPPABLE;
	bReplicates = true;
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

void AEquippable::Init_Implementation(UInventoryComponent * InventoryComp)
{
	Super::Init_Implementation(InventoryComp);

	totalStats = baseStats;
}

void AEquippable::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEquippable, baseStats);
	DOREPLIFETIME(AEquippable, totalStats);
	DOREPLIFETIME(AEquippable, ItemEquipPassive);

}
