// Fill out your copyright notice in the Description page of Project Settings.

#include "Status.h"

#include "Test2D.h"
#include "PaperFlipbookComponent.h"
#include "ParticleSystemActor.h"
#include "SkillHandlerComponent.h"
#include "Test2DCharacter.h"
#include "AttributeComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATest2DAlly::ATest2DAlly()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	if (Sphere)
	{
		SetRootComponent(Sphere);
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		Sphere->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
		Sphere->SetSphereRadius(6.0f);
	}
	Sprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperFlipBook"));
	if (Sprite)
	{
		Sprite->SetupAttachment(Sphere);
		Sprite->AlwaysLoadOnClient = true;
		Sprite->AlwaysLoadOnServer = true;
		Sprite->bOwnerNoSee = false;
		Sprite->bAffectDynamicIndirectLighting = true;
		Sprite->PrimaryComponentTick.TickGroup = TG_PrePhysics;

		Sprite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Sprite->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		Sprite->bGenerateOverlapEvents = false;
	}

	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	if (ParticleSystem)
	{
		ParticleSystem->SetupAttachment(Sphere);
	}

	Sprite->SetIsReplicated(true);

	bReplicates = true;

	if (statusName.IsEmpty())
		statusName = this->GetClass()->GetName();

	if (statusDescription.IsEmpty())
		statusDescription = this->GetClass()->GetName() + "_Description";
}

// Called when the game starts or when spawned
void ATest2DAlly::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATest2DAlly::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority())
		return;


	for (int i = stacks.Num() - 1; i >= 0; --i)
	{
		if (stacks[i] != -1.0f)
		{
			stacks[i] = stacks[i] - DeltaTime;
			if (stacks[i] <= 0.0f)
			{
				stacks.RemoveAt(i);

				if (stacks.Num() != 0)
					appliedTo->RecalculateTotalStats();

				StackDecrFunc();
			}
		}
	}

	currTickInterval -= DeltaTime;
	if (currTickInterval <= 0.0f)
	{
		++currTicks;
		TickFunc();
		currTickInterval = TickFuncInterval;

		if (maxTicks != -1 && currTicks >= maxTicks)
		{
			MaxTickFunc();
		}
		if (stacks.Num() >= maxStacks)
		{
			MaxStackFunc();
		}
	}

	if (stacks.Num() == 0)
	{
		Destroy();
	}
}
void ATest2DAlly::OnInit_Implementation()
{
}
void ATest2DAlly::TickFunc_Implementation()
{
}
void ATest2DAlly::MaxStackFunc_Implementation()
{
}
void ATest2DAlly::MaxTickFunc_Implementation()
{
}
void ATest2DAlly::StackDecrFunc_Implementation()
{
}
void ATest2DAlly::Init(ATest2DCharacter * _appliedBy, ATest2DCharacter * _appliedTo, float lifeTime)
{
	if (!HasAuthority())
		return;

	appliedBy = _appliedBy;
	appliedTo = _appliedTo;
	stacks.Add(lifeTime);

	if (appliedTo && bAttachToAppliedTo)
		GetRootComponent()->AttachToComponent(appliedTo->GetRootComponent(),FAttachmentTransformRules::SnapToTargetIncludingScale);

	OnInit();
}
void ATest2DAlly::Destroyed()
{
	if (!HasAuthority())
		return;

	if (appliedTo)
	{
		appliedTo->AttributeComponent->removeStatus(this);
	}
	Super::Destroyed();
}

void ATest2DAlly::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATest2DAlly, TickFuncInterval);
	DOREPLIFETIME(ATest2DAlly, currTickInterval);

	DOREPLIFETIME(ATest2DAlly, appliedBy);
	DOREPLIFETIME(ATest2DAlly, appliedTo);


}