// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "AbstractSkill.h"
#include "SkillHandlerComponent.generated.h"


UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEST2D_API USkillHandlerComponent : public UActorComponent
{
	GENERATED_BODY()


private:
	bool ReplicateSubobjects(UActorChannel* channel, FOutBunch* bunch, FReplicationFlags* repFlags) override;
	ESkillSlotTypes slotToUse = ESkillSlotTypes::SS_None;
	UPROPERTY(Replicated)
		int skillCounter = 0;
public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DamageTextUse")
		int getSkillUseID() { return skillCounter; }

	class ATest2DCharacter * Test2DCharacter;

	// Sets default values for this component's properties
	USkillHandlerComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(NetMulticast, WithValidation, Reliable, BlueprintCallable, Category = "Skills")
		void setSkill(ESkillSlotTypes skillSlot, UAbstractSkill * skill);

	UFUNCTION(BlueprintCallable, Category = "Skills")
		UAbstractSkill * getCurrSkill();

	class UPaperFlipbook * getCurrSkillAnim();

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "Skills")
		void tryUseSkillSlot(ESkillSlotTypes slot);

	//UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "Skills")
	UFUNCTION(/*NetMulticast, WithValidation, Reliable, */BlueprintCallable, Category = "Skills")
		void update(float DeltaTime);

	UFUNCTION(/*NetMulticast, WithValidation, Reliable,*/ BlueprintCallable, Category = "Skills")
		void LoadSkills(ATest2DCharacter * character);
	UFUNCTION(BlueprintCallable, Category = "Skills")
		void ReInitSkills();

	UFUNCTION(BlueprintCallable, Category = "Skills")
		void Init(ATest2DCharacter * character);

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Skills")
		TArray<UAbstractSkill*> skillSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
		TArray<TSubclassOf<UAbstractSkill>> skillSlotsClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
		FVector LastMoveDir;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills")
		FVector ToTargetDir;



protected:

	UPROPERTY(Replicated)
		ESkillSlotTypes currSlotInUse = ESkillSlotTypes::SS_None;


};
