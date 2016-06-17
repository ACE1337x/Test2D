// Fill out your copyright notice in the Description page of Project Settings.

#include "Test2D.h"
#include "ParticleSystemActor.h"


// Sets default values
AParticleSystemActor::AParticleSystemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	if (ParticleSystem)
	{
		SetRootComponent(ParticleSystem);
	}

}

// Called when the game starts or when spawned
void AParticleSystemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AParticleSystemActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

