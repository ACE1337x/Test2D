// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseProjectile.h"

#include "Test2D.h"

#include "AbstractSkill.h"

#include "../Source/Runtime/Engine/Classes/GameFramework/ProjectileMovementComponent.h"
#include "PaperFlipbookComponent.h"
#include "AttributeComponent.h"
#include "Test2DCharacter.h"
#include "ParticleSystemActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABaseProjectile::ABaseProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	if (Sphere)
	{
		SetRootComponent(Sphere);
		Sphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		Sphere->SetSphereRadius(6.0f);
	}
	Sprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperFlipBook"));
	//Sprite_Back = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperFlipBook"));
	if (Sprite)
	{
		Sprite->SetupAttachment(Sphere);
		//Sprite_Back->AttachParent = Sprite;

		Sprite->AlwaysLoadOnClient = true;
		//Sprite_Back->AlwaysLoadOnClient = true;

		Sprite->AlwaysLoadOnServer = true;
		//Sprite_Back->AlwaysLoadOnServer = true;

		Sprite->bOwnerNoSee = false;
		//Sprite_Back->bOwnerNoSee = false;

		Sprite->bAffectDynamicIndirectLighting = true;
		//Sprite_Back->bAffectDynamicIndirectLighting = true;

		Sprite->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		//Sprite_Back->PrimaryComponentTick.TickGroup = TG_PrePhysics;

		Sprite->SetCollisionProfileName(TEXT("CharacterMesh"));
		//Sprite_Back->SetCollisionProfileName(TEXT("CharacterMesh"));

		Sprite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Sprite_Back->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		Sprite->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		//Sprite_Back->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		Sprite->bGenerateOverlapEvents = false;
		//Sprite_Back->bGenerateOverlapEvents = false;

		//Sprite_Back->RelativeLocation = FVector(Sprite->RelativeLocation.X, -Sprite->RelativeLocation.Y, Sprite->RelativeLocation.Z);
		//Sprite_Back->SetFlipbook(Sprite->GetFlipbook());
	}

	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	if (ParticleSystem)
	{

		ParticleSystem->SetupAttachment(Sphere);
	}

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	if (ProjectileMovement)
	{
		ProjectileMovement->SetUpdatedComponent(Sphere);
		ProjectileMovement->ProjectileGravityScale = 0.1f;
		ProjectileMovement->InitialSpeed = 1000;
		ProjectileMovement->MaxSpeed = 1000;
	}


	Sprite->SetIsReplicated(true);
	//Sprite_Back->SetIsReplicated(true);

	bReplicates = true;


}

// Called when the game starts or when spawned
void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	initialLocation = GetActorLocation();
}

// Called every frame
void ABaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (FVector::DistSquared(GetActorLocation(), initialLocation) > (MaxProjectileDistance * MaxProjectileDistance))
	{
		StopSpinning(nullptr);
		Destroy();
	}
}

void ABaseProjectile::Destroyed()
{
	if (!HasAuthority())
		return;

	if (skill)
		skill->projectiles.Remove(this);

	Super::Destroyed();
}

void ABaseProjectile::ProjectileHit_Implementation(AActor * other, FVector locationOfCollision, bool &stopProjectile)
{
	if (!HasAuthority())
		return;

	if (stopped)
		return;

	if (other && skill && skill->Test2DCharacter != other && other->GetClass()->IsChildOf(ATest2DCharacter::StaticClass()))
	{
	
		ATest2DCharacter * otherCharacter = (ATest2DCharacter*)other;

		if (skill->Test2DCharacter->canTarget(otherCharacter) && (actorsHit.Find(other) == -1))
		{
			actorsHit.Add(other);

			otherCharacter->TakeDamageTest(
				skill->Test2DCharacter,
				skill,
				skill->skillUseID,
				EDmgType::DT_Skill,
				skill->skillData,
				skill->Test2DCharacter->totalStats,
				locationOfCollision,
				skill->Test2DCharacter->damageTextActorStyle
				);

			if (numPierces > 0)
			{
				numPierces--;
				stopProjectile = false;
			}
			else
			{
				if (otherCharacter->bIsEvading || otherCharacter->bIsInvuln)
					stopProjectile = false;
				else
				{
					StopSpinning(other);
					stopProjectile = true;
				}
			}
		}
	}
}
void ABaseProjectile::StopSpinning_Implementation(AActor* attachTo)
{
	if (stopped == false)
	{
		SetLifeSpan(lifetimeOnHit);
		stopped = true;
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->Deactivate();
		if (attachTo)
		{
			AttachToActor(attachTo, FAttachmentTransformRules::KeepWorldTransform);
		}
	}
}
void ABaseProjectile::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	//DOREPLIFETIME(ATest2DCharacter, SkillHandler);
	//DOREPLIFETIME(ATest2DCharacter, AttributeComponent);

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseProjectile, stopped);



}