// Fill out your copyright notice in the Description page of Project Settings.

#include "Test2D.h"
#include "Item.h"
#include "InventoryComponent.h"
#include "AttributeComponent.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UInventoryComponent::Init(UAttributeComponent * OwnerAttribComp)
{
	InventoryItems.SetNumZeroed(MaxInventorySize);
	InventoryLoadData.SetNumZeroed(MaxInventorySize);

	AttributeComponent = OwnerAttribComp;

	EquippedItems.SetNum(6);
	
	for (int i = 0; i < InventoryLoadData.Num(); i++)
	{
		if (InventoryLoadData[i].ItemClass)
		{
			InventoryItems[i] = GetOwner()->GetWorld()->SpawnActor<AItem>(InventoryLoadData[i].ItemClass);
			InventoryItems[i]->Init(OwnerAttribComp, this);
		}
	}

}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

void UInventoryComponent::SwapItems(int LHS, int RHS)
{
	if (LHS < 0 || RHS < 0)
		return;

	InventoryItems.Swap(LHS, RHS);
	onUpdateInventoryDelegate.Broadcast();
}

void UInventoryComponent::EquipItem(AEquippable * Equip)
{
	if (!Equip)
		return;

	if (!AttributeComponent)
		return;

	AEquippable * EquippedSlotItem = EquippedItems[(uint8)Equip->EquipType];
	

	if (AttributeComponent->EquippedItems.Find(Equip) != INDEX_NONE)
	{
		//unequip
		AttributeComponent->EquippedItems.Remove(Equip);
		InventoryItems[InventoryItems.Find(nullptr)] = Equip;

		EquippedItems[(uint8)Equip->EquipType] = nullptr;
	}
	else
	{
		//equip
		InventoryItems[InventoryItems.Find(Equip)] = EquippedSlotItem;
		AttributeComponent->EquippedItems.Add(Equip);

		EquippedItems[(uint8)Equip->EquipType] = Equip;
	}
	AttributeComponent->RecalculateTotalStats();
	onUpdateInventoryDelegate.Broadcast();
}

//void UInventoryComponent::UseItem(AItem * itemToUse)
//{
//	UAttributeComponent* AttribComp = ((UAttributeComponent*)GetOwner()->GetComponentByClass(UAttributeComponent::StaticClass()));
//	
//	if (itemToUse)
//		itemToUse->UseItem(AttribComp, this);
//}
