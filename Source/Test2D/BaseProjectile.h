// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

class UAbstractSkill;
class UPaperFlipbookComponent;
class UProjectileMovementComponent;

UCLASS()
class TEST2D_API ABaseProjectile : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
		bool stopped = false;
	// Sets default values for this actor's properties
	ABaseProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
		UProjectileMovementComponent * ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
		UPaperFlipbookComponent * Sprite;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	//	UPaperFlipbookComponent * Sprite_Back;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
		USphereComponent * Sphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
		UParticleSystemComponent * ParticleSystem;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	virtual void Destroyed() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
		UAbstractSkill * skill;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
		TArray<AActor*> actorsHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EntityAttributes)
		float lifetimeOnHit = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EntityAttributes)
		int numPierces = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EntityAttributes)
		FVector initialLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EntityAttributes)
		float MaxProjectileDistance;

	void ProjectileHit_Implementation(AActor * other, FVector locationOfCollision, bool &stopProjectile);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Skill")
		void ProjectileHit(AActor * other, FVector locationOfCollision, bool &stopProjectile);

	void StopSpinning_Implementation(AActor * attachTo);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Skill")
		void StopSpinning(AActor * attachTo);
};

USTRUCT(BlueprintType)
struct TEST2D_API FProjectileSpawnData
{
	GENERATED_USTRUCT_BODY();

	FProjectileSpawnData()
	{
		projectileClass = 0;
		projectileRotation = FRotator::ZeroRotator;
		currHitToSpawn = -1;
	}

	FProjectileSpawnData(TSubclassOf<ABaseProjectile> _projectileClass, FRotator _projectileRotation, int _currHitModToSpawn)
	{
		projectileClass = _projectileClass;
		projectileRotation = _projectileRotation;
		currHitToSpawn = _currHitModToSpawn;
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Data")
		TSubclassOf<ABaseProjectile> projectileClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Data")
		FRotator projectileRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status Data")
		int currHitToSpawn;
};
