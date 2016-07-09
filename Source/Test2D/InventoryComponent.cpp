// Fill out your copyright notice in the Description page of Project Settings.

#include "Test2D.h"
#include "Item.h"
#include "InventoryComponent.h"
#include "Test2DCharacter.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UInventoryComponent::Init(ATest2DCharacter * _Test2DCharacter)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, GetOwner()->HasAuthority() ? FColor::Red : FColor::Cyan, GetOwner()->HasAuthority() ? "Server" : "Client");

	InventoryItems.SetNumZeroed(MaxInventorySize);
	InventoryLoadData.SetNumZeroed(MaxInventorySize);

	Test2DCharacter = _Test2DCharacter;

	EquippedItems.SetNum(6);
	
	for (int i = 0; i < InventoryLoadData.Num(); i++)
	{
		if (InventoryLoadData[i].ItemClass)
		{
			InventoryItems[i] = GetOwner()->GetWorld()->SpawnActor<AItem>(InventoryLoadData[i].ItemClass);
			InventoryItems[i]->Init(this);
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

void UInventoryComponent::SwapItems_Implementation(int LHS, int RHS)
{
	if (LHS < 0 || RHS < 0)
		return;

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Swapping Item " + FString::FromInt(LHS) + " <-> " + FString::FromInt(RHS));

	InventoryItems.Swap(LHS, RHS);
}
bool UInventoryComponent::SwapItems_Validate(int LHS, int RHS) { return true; }

void UInventoryComponent::EquipItem_Implementation(AEquippable * Equip)
{
	if (!GetOwner()->HasAuthority())
		return;

	if (!Equip)
		return;

	if (!Test2DCharacter)
		return;

	AEquippable * EquippedSlotItem = EquippedItems[(uint8)Equip->EquipType];

	if (EquippedItems.Find(Equip) != INDEX_NONE)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Unequipping Item");
		//unequip
		InventoryItems[InventoryItems.Find(nullptr)] = Equip;

		EquippedItems[(uint8)Equip->EquipType] = nullptr;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Equipping Item");
		//equip
		InventoryItems[InventoryItems.Find(Equip)] = EquippedSlotItem;

		EquippedItems[(uint8)Equip->EquipType] = Equip;
	}

	Test2DCharacter->RecalculateTotalStats();
}


void UInventoryComponent::RecalculateTotalStats_Implementation()
{
	totalStats = FPlayerAttributes();

	for (int i = 0; i < EquippedItems.Num(); i++)
	{
		if (EquippedItems[i])
			totalStats = UStructHelperFunctions::AddAttributes(EquippedItems[i]->totalStats, totalStats);
	}
}
bool UInventoryComponent::RecalculateTotalStats_Validate()
{
	return true;
}

bool UInventoryComponent::EquipItem_Validate(AEquippable * Equip)
{
	return true;
}

//void UInventoryComponent::UseItem(AItem * itemToUse)
//{
//	UAttributeComponent* AttribComp = ((UAttributeComponent*)GetOwner()->GetComponentByClass(UAttributeComponent::StaticClass()));
//	
//	if (itemToUse)
//		itemToUse->UseItem(AttribComp, this);
//}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	DOREPLIFETIME(UInventoryComponent, EquippedItems);
	DOREPLIFETIME(UInventoryComponent, InventoryItems);

	DOREPLIFETIME(UInventoryComponent, InventoryLoadData);

	DOREPLIFETIME(UInventoryComponent, Currency);
	DOREPLIFETIME(UInventoryComponent, MaxInventorySize);
	DOREPLIFETIME(UInventoryComponent, SelectedIndex);

	DOREPLIFETIME(UInventoryComponent, totalStats);





}