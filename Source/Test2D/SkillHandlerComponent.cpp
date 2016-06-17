// Fill out your copyright notice in the Description page of Project Settings.

#include "Test2D.h"
#include "SkillHandlerComponent.h"
#include "AttributeComponent.h"
#include "Engine.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"


// Sets default values for this component's properties
USkillHandlerComponent::USkillHandlerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	skillSlots.SetNum((uint8)ESkillSlotTypes::SS_None);
	skillSlotsClasses.SetNum((uint8)ESkillSlotTypes::SS_None);
	//bReplicates = true;
	// ...
	//OnTestDelegate.AddDynamic(this, &USkillHandlerComponent::setSkill);
}


// Called when the game starts
void USkillHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
}
void USkillHandlerComponent::Init(UAttributeComponent*attribComp)
{
	AttributeComponent = attribComp;

	if (AttributeComponent)
	{
		LoadSkills(AttributeComponent);
		AttributeComponent->onRespawnDelegate.AddDynamic(this, &USkillHandlerComponent::ReInitSkills);
	}
}
// ...
void USkillHandlerComponent::LoadSkills/*_Implementation*/(UAttributeComponent * attribComp)
{
	if (!GetOwner()->HasAuthority())
		return;

	if (attribComp != AttributeComponent && attribComp != nullptr)
	{
		AttributeComponent = attribComp;
	}

	skillSlots.Empty();
	skillSlots.SetNum((uint8)ESkillSlotTypes::SS_None, true);

	for (int i = 0; i < skillSlotsClasses.Num(); i++)
	{
		if (skillSlotsClasses[i])
			setSkill((ESkillSlotTypes)i, NewObject<UAbstractSkill>(GetOwner(), skillSlotsClasses[i]));
	}
	ReInitSkills();

}
void USkillHandlerComponent::ReInitSkills()
{
	for (int i = 0; i < skillSlots.Num(); i++)
	{
		if (skillSlots[i])
			skillSlots[i]->Init(GetOwner(), AttributeComponent, AttributeComponent ? AttributeComponent->GetOwner() : nullptr);
	}
}
//bool USkillHandlerComponent::LoadSkills_Validate()
//{
//	return true;
//}


// Called every frame
void USkillHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	update(DeltaTime);
}

void USkillHandlerComponent::update/*_Implementation*/(float DeltaTime)
{
	if (!GetOwner()->HasAuthority())
		return;

	if (slotToUse != ESkillSlotTypes::SS_None)
	{
		if (skillSlots[(uint8)slotToUse] != nullptr &&
			(currSlotInUse == ESkillSlotTypes::SS_None || (currSlotInUse != slotToUse && skillSlots[(uint8)currSlotInUse]->canSkillBeCancelled() && skillSlots[(uint8)slotToUse]->canCancelSkills())) &&
			skillSlots[(uint8)slotToUse]->canBeUsed())
		{
			if (currSlotInUse != ESkillSlotTypes::SS_None)
				skillSlots[(uint8)currSlotInUse]->cancelSkill();

			currSlotInUse = slotToUse;

			skillSlots[(uint8)currSlotInUse]->activateSkill();
		}
		slotToUse = ESkillSlotTypes::SS_None;
	}
	if (currSlotInUse != ESkillSlotTypes::SS_None)
	{
		getCurrSkill()->update(DeltaTime, ToTargetDir, LastMoveDir);

		if (!getCurrSkill()->isSkillUpdating())
		{
			currSlotInUse = ESkillSlotTypes::SS_None;
			//currSkillAnimName = null;
		}
	}
	for (int i = 0; i < (uint8)ESkillSlotTypes::SS_None; i++)
	{
		if (skillSlots[i] && skillSlots[i]->IsValidLowLevel())
		{
			skillSlots[i]->decrementCooldown(DeltaTime);

			if (skillSlots[i]->isSkillComplete() && skillSlots[i]->nextSkillInChain != nullptr) // check if cooldown is done to go to next skill in skill chain
				setSkill((ESkillSlotTypes)i, NewObject<UAbstractSkill>(GetOwner(), skillSlots[i]->nextSkillInChain));
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::FromInt((uint8)slot));

}
//bool USkillHandlerComponent::update_Validate(float DeltaTime)
//{
//	return true;
//}

void USkillHandlerComponent::setSkill_Implementation(ESkillSlotTypes skillSlot, UAbstractSkill * skill)
{
	if (skillSlots[(uint8)skillSlot])
		skillSlots[(uint8)skillSlot]->Destroy();

	if (skill)
		skill->Init(GetOwner(), AttributeComponent, AttributeComponent ? AttributeComponent->GetOwner() : nullptr);
	skillSlots[(uint8)skillSlot] = skill;
	//OnTestDelegate.Broadcast(skillSlot, skillSlots[(uint8)skillSlot]);
}
bool USkillHandlerComponent::setSkill_Validate(ESkillSlotTypes skillSlot, UAbstractSkill * skill) {
	return true;
}


UAbstractSkill * USkillHandlerComponent::getCurrSkill() {
	if (currSlotInUse != ESkillSlotTypes::SS_None) {
		return skillSlots[(uint8)currSlotInUse];
	}
	else {
		return nullptr;
	}
}
UPaperFlipbook * USkillHandlerComponent::getCurrSkillAnim() {
	UPaperFlipbook * anim = nullptr;
	if (getCurrSkill())
		anim = getCurrSkill()->skillAnimation;
	return anim;
}

void USkillHandlerComponent::tryUseSkillSlot_Implementation(ESkillSlotTypes slot) {
	slotToUse = slot;
}
bool USkillHandlerComponent::tryUseSkillSlot_Validate(ESkillSlotTypes slot)
{
	return true;
}

void USkillHandlerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USkillHandlerComponent, skillSlots);
	DOREPLIFETIME(USkillHandlerComponent, currSlotInUse);
	//DOREPLIFETIME(USkillHandlerComponent, OnTestDelegate);

}

bool USkillHandlerComponent::ReplicateSubobjects(UActorChannel* channel, FOutBunch* bunch, FReplicationFlags* repFlags) {
	check(channel && bunch && repFlags);

	bool wroteSomething = Super::ReplicateSubobjects(channel, bunch, repFlags);

	//	wroteSomething |= channel->ReplicateSubobject(UAbstractSkill, *bunch, *repFlags);
	//	return wroteSomething;

	for (UAbstractSkill * skill : skillSlots)
	{
		if (skill && skill->IsValidLowLevel())
		{
			wroteSomething |= channel->ReplicateSubobject(skill, *bunch, *repFlags);
		}
	}
	return wroteSomething;
}