// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Test2DEnums.h"
#include "GameFramework/Actor.h"
#include "DamageTextActor.generated.h"
UCLASS()
class TEST2D_API ADamageTextActor : public AActor
{
	GENERATED_BODY()


public:
	// Sets default values for this actor's properties
	ADamageTextActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DamageTextActor, meta = (AllowPrivateAccess = "true"))
		UBoxComponent * boxComponent;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Text Variables")
		EDmgType DmgType = EDmgType::DT_Skill;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Text Variables")
		bool CriticalHit = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Text Variables")
		float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Text Variables")
		float HitMaxHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Text Variables")
		float Lifetime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Text Variables")
		float RemainingLifetime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Text Variables")
		float Scale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Text Variables")
		class UAttributeComponent * DamageCauser;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Text Variables")
		int DmgTextSpawnID = 0;

};
