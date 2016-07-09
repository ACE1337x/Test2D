// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PlayerAttributes.h"
#include "Components/ActorComponent.h"
#include "DamageTextActor.h"
#include "AbstractSkillData.h"
#include "Status.h"
//#include <map>
#include "AttributeComponent.generated.h"
class UAbstractSkill;
class AEquippable;
class ATest2DCharacter;
//class AHpBarActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHUDEventDelegate);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEST2D_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	typedef TMap<ATest2DCharacter *, TArray<ATest2DAlly*>> TApplierMap;
	typedef TMap<TSubclassOf<ATest2DAlly>, TApplierMap> TStatusMap;

	//typedef std::map<UAttributeComponent *, TArray<ATest2DAlly*>> attribMap;
	//typedef std::map<TSubclassOf<ATest2DAlly>, attribMap> statusMap;

	//UPROPERTY(Replicated)
	TStatusMap StatusMap;

	UPROPERTY(Replicated)
		TArray<TSubclassOf<ATest2DAlly>> AppliedStatuses;
public:
	// Sets default values for this component's properties
	UAttributeComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	//	TSubclassOf<AHpBarActor> HpBarClass;
	//UPROPERTY(BlueprintReadOnly, Category = Attributes)
	//	AHpBarActor * HpBar;

	class ATest2DCharacter * Test2DCharacter;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		void Init(ATest2DCharacter * _Test2DCharacter);

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Attributes)
		FPlayerAttributes totalStats;

	//UFUNCTION(NetMulticast, WithValidation, Reliable, BlueprintCallable, Category = Attributes)
	//	virtual void NetMulticastRPC_TogglePVP(bool pvp);

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = Attributes)
		void RecalculateTotalStats();

	UFUNCTION(BlueprintCallable, Category = "Status")
		TArray<ATest2DAlly *> applyStatus(TArray<FStatusApplicationData> statusToApply, ATest2DCharacter * appliedBy);

	ATest2DAlly * spawnStatus(ATest2DCharacter * appliedBy, FStatusApplicationData allySpawnData);

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "Status")
		void removeStatus(ATest2DAlly * statusToRemove);

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "Status")
		void clearStatuses();

	// TODO move this to a helper functions static class
	UFUNCTION(BlueprintCallable, Category = "Status")
		UTexture2D * getStatusIcon(TSubclassOf<ATest2DAlly> statusClass);

	UFUNCTION(BlueprintCallable, Category = "Status")
		int getTotalStacksFromStatus(TSubclassOf<ATest2DAlly> statusClass);

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "Status")
		void updateStatusBarStatuses(bool checkForShowOnStatusBar);

	UFUNCTION(BlueprintCallable, Category = "Status")
		void updateStatusBar_RepNotify();

	UPROPERTY(ReplicatedUsing = updateStatusBar_RepNotify, BlueprintReadOnly, Category = Attributes)
		TArray<FStatusApplicationData> statusBarStatuses;

	UPROPERTY(Replicated, BlueprintAssignable, Category = "Test")
		FHUDEventDelegate onUpdateStatusBarDelegate;
};

