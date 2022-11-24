// Fill out your copyright notice in the Description page of Project Settings.

#include "AttributeComponent.h"

#include "Test2D.h"
#include "Test2DCharacter.h"
#include "Test2DEnums.h"
#include "AbstractSkill.h"
#include "Equippable.h"
#include "Net/UnrealNetwork.h"

//#include "HpBarActor.h"
//#include "../Source/Runtime/Engine/Classes/Components/SceneComponent.h"

UAttributeComponent::UAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
	// ...
	//bReplicates = true;
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	//if (GetOwner()->HasAuthority())
	//{
	//	totalStats = UStructHelperFunctions::AddAttributes(baseStats, totalStats);
	//	currJumps = totalStats.MaxJumps;
	//}
	//if (GetOwner()->HasAuthority())
	//	Respawn();
}
void UAttributeComponent::Init(ATest2DCharacter * _Test2DCharacter)
{
	Test2DCharacter = _Test2DCharacter;
}

// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttributeComponent::RecalculateTotalStats_Implementation()
{
	//if (!GetOwner()->HasAuthority())
	//	return;

	totalStats = FPlayerAttributes();

	TStatusMap & tempMap = StatusMap;

	for (auto StatusMapIter = tempMap.CreateIterator(); StatusMapIter; ++StatusMapIter)
	{
		for (auto AttribIter = StatusMapIter->Value.CreateIterator(); AttribIter; ++AttribIter)
		{
			for (int Array_i = AttribIter->Value.Num() - 1; Array_i >= 0; --Array_i)
			{
				totalStats = UStructHelperFunctions::AddAttributes(
					UStructHelperFunctions::ScaleAttributes(
						AttribIter->Value[Array_i]->statModifier,
						AttribIter->Value[Array_i]->stacks.Num()),
					totalStats);
			}
		}
	}
	updateStatusBarStatuses(true);
}
bool UAttributeComponent::RecalculateTotalStats_Validate() { return true; }

ATest2DAlly * UAttributeComponent::spawnStatus(ATest2DCharacter * appliedBy, FStatusApplicationData statusApplicationData)
{
	FActorSpawnParameters fSpawn;
	fSpawn.Owner = GetOwner();
	fSpawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ATest2DAlly * toSpawn = GetOwner()->GetWorld()->SpawnActorDeferred<ATest2DAlly>(
		statusApplicationData.statusClass,
		GetOwner()->GetTransform(),
		GetOwner(),
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	toSpawn->Init(appliedBy, Test2DCharacter, statusApplicationData.statusLifetime);

	toSpawn->FinishSpawning(GetOwner()->GetTransform());

	StatusMap[statusApplicationData.statusClass][appliedBy].Add(toSpawn);

	return toSpawn;
}

TArray<ATest2DAlly *> UAttributeComponent::applyStatus(TArray<FStatusApplicationData> statusesToApply, ATest2DCharacter * appliedBy)
{
	TArray<ATest2DAlly *> status;

	if (statusesToApply.Num() == 0)
		return status;

	if (!Test2DCharacter->bIsAlive)
		return status;

	for (int data_i = 0; data_i < statusesToApply.Num(); data_i++)
	{
		FStatusApplicationData &statusApplicationData = statusesToApply[data_i];
		ATest2DAlly * statusDefaults = statusApplicationData.statusClass.GetDefaultObject();

		if (!StatusMap.Contains(statusApplicationData.statusClass))
			StatusMap.Add(statusApplicationData.statusClass);

		if (!StatusMap[statusApplicationData.statusClass].Contains(appliedBy))
			StatusMap[statusApplicationData.statusClass].Add(appliedBy);

		TArray<ATest2DAlly *> &appliedByMapIndex = StatusMap[statusApplicationData.statusClass][appliedBy];

		int stacksToApply = (statusApplicationData.numStacks > statusDefaults->maxStacks) ? statusDefaults->maxStacks : statusApplicationData.numStacks;

		if (statusApplicationData.statusClass)
		{
			for (int numAllies_i = 0; numAllies_i < stacksToApply; numAllies_i++)
			{
				if (!statusDefaults->bStackInternal)
				{
					status.Add(spawnStatus(appliedBy, statusApplicationData));

					if (appliedByMapIndex.Num() > statusDefaults->maxStacks)
					{
						if (appliedByMapIndex[0])
							appliedByMapIndex[0]->Destroy();
					}
				}
				else
				{
					if (appliedByMapIndex.Num() == 0)
					{
						status.Add(spawnStatus(appliedBy, statusApplicationData));
					}
					else
					{
						appliedByMapIndex[0]->stacks.Add(statusApplicationData.statusLifetime);
						status.Add(appliedByMapIndex[0]);

						if (appliedByMapIndex[0]->stacks.Num() > statusDefaults->maxStacks)
							appliedByMapIndex[0]->stacks.RemoveAt(0, 1, true);
					}
				}
			}
			AppliedStatuses.AddUnique(statusApplicationData.statusClass);
		}
	}
	Test2DCharacter->RecalculateTotalStats();
	return status;
}
void UAttributeComponent::removeStatus_Implementation(ATest2DAlly * statusToRemove)
{
	if (!statusToRemove)
		return;

	StatusMap[statusToRemove->GetClass()][statusToRemove->appliedBy].Remove(statusToRemove);
	if (StatusMap[statusToRemove->GetClass()].Num() == 0/*.empty()*/ ||
		StatusMap[statusToRemove->GetClass()][statusToRemove->appliedBy].Num() == 0)
		AppliedStatuses.Remove(statusToRemove->GetClass());

	Test2DCharacter->RecalculateTotalStats();
}
bool UAttributeComponent::removeStatus_Validate(ATest2DAlly * statusToRemove) { return true; }


void UAttributeComponent::clearStatuses_Implementation()
{
	//statusMap::reverse_iterator StatusMapIter;
	//attribMap::reverse_iterator AttribIter;

	for (auto StatusMapIter = StatusMap.CreateIterator()/*.rbegin()*/; StatusMapIter/* != StatusMap.rend()*/; ++StatusMapIter)
	{
		for (auto AttribIter = StatusMapIter->Value.CreateIterator()/*second.rbegin()*/; AttribIter/* != StatusMapIter->second.rend()*/; ++AttribIter)
		{
			for (int Array_i = AttribIter->Value/*second*/.Num() - 1; Array_i >= 0; --Array_i)
			{
				AttribIter->Value[Array_i]->Destroy();
			}
		}
		StatusMapIter->Value.Empty();
	}
	StatusMap.Empty();
	AppliedStatuses.Empty();

	Test2DCharacter->RecalculateTotalStats();
}
bool UAttributeComponent::clearStatuses_Validate() { return true; }

int UAttributeComponent::getTotalStacksFromStatus(TSubclassOf<ATest2DAlly> statusClass)
{
	int ret = 0;

	if (StatusMap.Contains(statusClass))
	{
		TApplierMap & tempMap = StatusMap[statusClass];
		//attribMap::iterator AttribIter;

		for (auto AttribIter = tempMap.CreateIterator()/*.begin()*/; AttribIter/* != tempMap.end()*/; ++AttribIter)
		{
			for (int Array_i = AttribIter->Value.Num() - 1; Array_i >= 0; --Array_i)
			{
				ret += AttribIter->Value[Array_i]->stacks.Num();
			}
		}
	}
	return ret;
}
void UAttributeComponent::updateStatusBar_RepNotify()
{
	onUpdateStatusBarDelegate.Broadcast();
}

void UAttributeComponent::updateStatusBarStatuses_Implementation(bool checkForShowOnStatusBar)
{
	if (!GetOwner()->HasAuthority())
		return;

	statusBarStatuses.Empty();

	int totalStacksFromClass = 0;

	TStatusMap & tempMap = StatusMap;

	for (int i = 0; i < AppliedStatuses.Num(); i++)
	{
		totalStacksFromClass = getTotalStacksFromStatus(AppliedStatuses[i]);
		if (totalStacksFromClass > 0)
		{
			statusBarStatuses.Add(FStatusApplicationData(AppliedStatuses[i], totalStacksFromClass));
		}
	}
	updateStatusBar_RepNotify();
}
bool UAttributeComponent::updateStatusBarStatuses_Validate(bool checkForShowOnStatusBar) { return true; }

UTexture2D * UAttributeComponent::getStatusIcon(TSubclassOf<ATest2DAlly> statusClass)
{
	return statusClass.GetDefaultObject()->StatusIcon;
}

void UAttributeComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAttributeComponent, totalStats);

	DOREPLIFETIME(UAttributeComponent, AppliedStatuses);

	DOREPLIFETIME(UAttributeComponent, statusBarStatuses);



	//DOREPLIFETIME(UAttributeComponent, totalSkillsUsed);


}