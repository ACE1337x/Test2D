// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PlayerAttributes.h"
#include "Status.generated.h"

class UPaperFlipbookComponent;
class UProjectileMovementComponent;
class USkillHandlerComponent;
class UAttributeComponent;

UCLASS()
class TEST2D_API ATest2DAlly : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATest2DAlly();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	virtual void Destroyed() override;

	//// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
		UTexture2D * StatusIcon;

	UPROPERTY(Category = "Ally", Replicated, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USkillHandlerComponent * SkillHandler;

	UPROPERTY(Category = "Ally", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UAttributeComponent * ownerAttribComp;

	UPROPERTY(Category = "Ally", Replicated, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UAttributeComponent * appliedTo;

	void OnInit_Implementation();
	UFUNCTION(/*NetMulticast, Reliable,*/ BlueprintNativeEvent, BlueprintCallable, Category = "Status")
		void OnInit();

	void TickFunc_Implementation();
	UFUNCTION(/*NetMulticast, Reliable,*/ BlueprintNativeEvent, BlueprintCallable, Category = "Status")
		void TickFunc();

	void MaxStackFunc_Implementation();
	UFUNCTION(/*NetMulticast, Reliable,*/ BlueprintNativeEvent, BlueprintCallable, Category = "Status")
		void MaxStackFunc();

	void MaxTickFunc_Implementation();
	UFUNCTION(/*NetMulticast, Reliable,*/ BlueprintNativeEvent, BlueprintCallable, Category = "Status")
		void MaxTickFunc();

	void StackDecrFunc_Implementation();
	UFUNCTION(/*NetMulticast, Reliable,*/ BlueprintNativeEvent, BlueprintCallable, Category = "Status")
		void StackDecrFunc();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Ally")
		float TickFuncInterval;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ally")
		float currTickInterval;

	UFUNCTION(BlueprintCallable, Category = "Allies")
		void Init(UAttributeComponent * _ownerAttribComp, UAttributeComponent * _appliedTo, float lifeTime);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ally", meta = (AllowPrivateAccess = "true"))
		UPaperFlipbookComponent * Sprite;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ally", meta = (AllowPrivateAccess = "true"))
		USphereComponent * Sphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ally", meta = (AllowPrivateAccess = "true"))
		UParticleSystemComponent * ParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ally")
		int maxTicks = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ally")
		int currTicks = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ally")
		int maxStacks = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ally")
		FPlayerAttributes statModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ally")
		bool bStackInternal = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ally")
		bool bShowOnStatusBar = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ally")
		bool bAttachToAppliedTo = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ally")
		TArray<float> stacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Data")
		FString statusName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Data")
		FString statusDescription;
};

USTRUCT(BlueprintType)
struct TEST2D_API FStatusApplicationData
{
	GENERATED_USTRUCT_BODY();

	FStatusApplicationData()
	{
		statusClass = 0;
		numStacks = 0;
		statusLifetime = 0;
		matchSkillDuration = 0;
	}

	FStatusApplicationData(TSubclassOf<ATest2DAlly> _statusClass, int _numStacks, float _statusLifetime = 0.0f, bool _matchSkillDuration = false)
	{
		statusClass = _statusClass;
		numStacks = _numStacks;
		statusLifetime = _statusLifetime;
		matchSkillDuration = _matchSkillDuration;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Data")
		TSubclassOf<ATest2DAlly> statusClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Data")
		int numStacks = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Data")
		float statusLifetime = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Data")
		bool matchSkillDuration = 0;
};