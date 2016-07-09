// Fill out your copyright notice in the Description page of Project Settings.

#include "Test2D.h"
#include "Test2DCharacter.h"
#include "DamageTextActor.h"


// Sets default values
ADamageTextActor::ADamageTextActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//bReplicates = true;

	boxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	if (boxComponent)
	{
		SetRootComponent(boxComponent);
		boxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		boxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		boxComponent->SetCollisionObjectType(ECC_GameTraceChannel3);
		boxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Overlap);

		boxComponent->SetBoxExtent(FVector(10, 10, 10));
	}
}


// Called when the game starts or when spawned
void ADamageTextActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ADamageTextActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADamageTextActor::Destroyed()
{
	if (GetOwner() && GetOwner()->GetClass()->IsChildOf(ATest2DCharacter::StaticClass()))
	{
		ATest2DCharacter * Test2DCharacter = (ATest2DCharacter *)GetOwner();

		Test2DCharacter->DamageTextArr[DamageCauser].Remove(this);
	}

	Super::Destroyed();
}