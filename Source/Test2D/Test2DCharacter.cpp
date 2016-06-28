// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Test2D.h"

#include "Kismet/KismetStringLibrary.h"
#include "Engine.h"

#include "Test2DCharacter.h"
#include "PaperFlipbookComponent.h"
#include "Components/TextRenderComponent.h"

#include "AttributeComponent.h"
#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);
//////////////////////////////////////////////////////////////////////////
// ATest2DCharacter

ATest2DCharacter::ATest2DCharacter()
{
	// Setup the assets
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> RunningAnimationAsset;
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> IdleAnimationAsset;
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> FallingAnimationAsset;
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> WallSlidingAnimationAsset;
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> JumpingAnimationAsset;
		ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> DeathAnimationAsset;

		FConstructorStatics()
			: RunningAnimationAsset		(TEXT("/Game/Assets/Flipbooks/Walking.Walking"))
			, IdleAnimationAsset		(TEXT("/Game/Assets/Flipbooks/Idle.Idle"))
			, FallingAnimationAsset		(TEXT("/Game/Assets/Flipbooks/Falling.Falling"))
			, WallSlidingAnimationAsset	(TEXT("/Game/Assets/Flipbooks/Falling.Falling"))
			, JumpingAnimationAsset		(TEXT("/Game/Assets/Flipbooks/Jumping.Jumping"))
			, DeathAnimationAsset		(TEXT("/Game/Assets/Flipbooks/Dead.Dead"))

		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	RunningAnimation		= ConstructorStatics.RunningAnimationAsset.Get();
	IdleAnimation			= ConstructorStatics.IdleAnimationAsset.Get();
	FallingAnimation		= ConstructorStatics.FallingAnimationAsset.Get();
	WallSlidingAnimation	= ConstructorStatics.WallSlidingAnimationAsset.Get();
	JumpingAnimation		= ConstructorStatics.JumpingAnimationAsset.Get();
	DeathAnimation			= ConstructorStatics.DeathAnimationAsset.Get();

	GetSprite()->SetFlipbook(IdleAnimation);

	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw	= false;
	bUseControllerRotationRoll	= false;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(10.0f);
	GetCapsuleComponent()->SetCapsuleRadius(5.0f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->CanCharacterStepUpOn = ECB_Yes;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 1.0f;
	GetCharacterMovement()->AirControl = 0.3f;
	GetCharacterMovement()->JumpZVelocity = 350.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 200.0f;
	GetCharacterMovement()->MaxFlySpeed = 200.0f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;
	GetCharacterMovement()->MaxStepHeight = 8.0f;

	AttributeComponent	= CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
	InventoryComponent	= CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	SkillHandler		= CreateDefaultSubobject<USkillHandlerComponent>(TEXT("SkillHandler"));

	wallSlideCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("wallSlideCollision"));
	if (wallSlideCollision)
	{
		//wallSlideCollision->AttachTo(GetRootComponent());
		wallSlideCollision->SetupAttachment(GetRootComponent());
		wallSlideCollision->SetBoxExtent(FVector(8.0f, 5.0f, 5.0f), true);

		wallSlideCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		wallSlideCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	if (SceneComponent)
	{
		SceneComponent->ComponentTags.Add("HpBarSlot");
		SceneComponent->SetupAttachment(GetCapsuleComponent());
	}

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	AttributeComponent->SetIsReplicated(true);
	SkillHandler->SetIsReplicated(true);

	bReplicates			= true;
	bReplicateMovement	= true;
	bAlwaysRelevant		= true;

}
void ATest2DCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComponent->Init();
	SkillHandler->Init(AttributeComponent);
	InventoryComponent->Init(AttributeComponent);

}
void ATest2DCharacter::TryUseSkill_Implementation(ESkillSlotTypes skillSlotToUse)
{
	if (!wallSliding && SkillHandler && AttributeComponent && AttributeComponent->isAlive())
	{
		SkillHandler->tryUseSkillSlot(skillSlotToUse);
	}
}
bool ATest2DCharacter::TryUseSkill_Validate(ESkillSlotTypes skillSlotToUse)
{
	return true;
}
void ATest2DCharacter::BeginPlay()
{
	Super::BeginPlay();

	//SceneComponent->SetRelativeLocation(FVector(0, 0, -(GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) + 7.0f));
}

//////////////////////////////////////////////////////////////////////////
// Animation

void ATest2DCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateCharacter();

	UpdateAnimation();
	
	if (Role == ROLE_AutonomousProxy)
		UpdateRPC(actorRotation, lookDir, DesiredAnimation);

}


//////////////////////////////////////////////////////////////////////////
// Input

void ATest2DCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
}

void ATest2DCharacter::Jump_Implementation()
{
	if (!HasAuthority())
		return;
	if (!AttributeComponent->isAlive())
		return;
	wantsToJump = true;
}
bool ATest2DCharacter::Jump_Validate()
{
	return true;
}


void ATest2DCharacter::Landed_Implementation(const FHitResult &Hit)
{
	Super::Landed(Hit);
	if (!HasAuthority())
		return;

	if (AttributeComponent != nullptr && AttributeComponent->IsValidLowLevel())
	{
		AttributeComponent->currJumps = AttributeComponent->totalStats.MaxJumps;
	}
}
bool ATest2DCharacter::Landed_Validate(const FHitResult &Hit)
{
	return true;
}

void ATest2DCharacter::MoveRight_Implementation(float Value)
{
	if (!HasAuthority())
		return;
	if (!AttributeComponent->isAlive())
		return;
	moveRightDir = Value;
}
bool ATest2DCharacter::MoveRight_Validate(float Value)
{
	return true;
}
void ATest2DCharacter::MoveUp_Implementation(float Value)
{
	if (!HasAuthority())
		return;
	if (!AttributeComponent->isAlive())
		return;
	moveUpDir = Value;
}
bool ATest2DCharacter::MoveUp_Validate(float Value)
{
	return true;
}

void ATest2DCharacter::UpdateCharacter/*_Implementation*/()
{
	if (!AttributeComponent->isAlive())
		return;

	FVector LastMoveDir = FVector(SkillHandler->ToTargetDir.X, 0.f, 0.f);
	LastMoveDir.Normalize();

	if (!SkillHandler || !SkillHandler->getCurrSkill() || !SkillHandler->getCurrSkill()->isMovementSkill())
	{
		moveInput = !FMath::IsNearlyZero(moveRightDir);

		if (moveInput)
		{
			if (moveRightDir < 0.0f)
			{
				actorRotation	= FRotator(0.0, 180.0f, 0.0f);
				LastMoveDir		= FVector(-1.0f, 0.f, 0.f);
			}
			else if (moveRightDir > 0.0f)
			{
				actorRotation	= FRotator(0.0, 0.0f, 0.0f);
				LastMoveDir		= FVector(1.0f, 0.f, 0.f);
			}
			AddMovementInput(FVector(1.0f, 0.0f, 0.0f), moveRightDir);
		}
		if (!moveInput || SkillHandler->getCurrSkill() && SkillHandler->getCurrSkill()->isCrosshairAimed())
		{
			if (lookDir.X < 0.0f)
			{
				actorRotation	= FRotator(0.0, 180.0f, 0.0f);
			}

			else if (lookDir.X > 0.0f)
			{
				actorRotation	= FRotator(0.0, 0.0f, 0.0f);
			}
		}
		SkillHandler->LastMoveDir = LastMoveDir;
	}
	else
		moveInput = false;

	SkillHandler->ToTargetDir = lookDir;

	falling = (GetVelocity().Z < 0.0f);
	jumping = (GetVelocity().Z > 0.0f);

	wallSliding = false;

	FVector averageHitNormals(0, 0, 0);
	if (!FMath::IsNearlyZero(GetVelocity().Z) && !SkillHandler->getCurrSkill())
	{
		TArray<FHitResult> hitResults;
		FCollisionQueryParams queryParams;
		GetWorld()->SweepMultiByObjectType(
			hitResults,
			wallSlideCollision->GetComponentLocation(),
			wallSlideCollision->GetComponentLocation() + GetActorForwardVector(),
			wallSlideCollision->GetComponentRotation().Quaternion(),
			ECollisionChannel::ECC_WorldStatic,
			wallSlideCollision->GetCollisionShape(),
			queryParams);

		if (hitResults.Num() > 0)
		{
			for (size_t i = 0; i < hitResults.Num(); i++)
				averageHitNormals += hitResults[i].Normal;

			averageHitNormals.Normalize();
			wallSliding = true;

			if (falling)
			{
				float fallCoeff = FMath::GetMappedRangeValueClamped(FVector2D(1.0f, -1.0f), FVector2D(0.8f, 0.2f), moveUpDir);
				LaunchCharacter(FVector(0.0f, 0.0f, -GetVelocity().Z * fallCoeff), false, false);
			}

			if (averageHitNormals.X > 0.0f)
				actorRotation = FRotator(0.0, 180.0f, 0.0f);
			else if (averageHitNormals.X < 0.0f)
				actorRotation = FRotator(0.0, 0.0f, 0.0f);
		}
	}

	if (wantsToJump)
	{
		wantsToJump = false;
		if ((SkillHandler->getCurrSkill() && !SkillHandler->getCurrSkill()->isMovementSkill()) ||
			!SkillHandler->getCurrSkill())
		{
			if (wallSliding)
			{
				wallSliding = false;
				FVector launchDir = averageHitNormals;

				launchDir.Z = GetCharacterMovement()->JumpZVelocity;
				launchDir.X = GetCharacterMovement()->JumpZVelocity * 1.0f * ((launchDir.X > 0) ? 1.0f : -1.0f);
				LaunchCharacter(launchDir, true, true);

			}
			else if (AttributeComponent != nullptr && AttributeComponent->IsValidLowLevel() && AttributeComponent->currJumps > 0)
			{
				LaunchCharacter(FVector(0, 0, GetCharacterMovement()->JumpZVelocity), false, true);
				AttributeComponent->currJumps--;
			}
		}
	}

	SetActorRotation(actorRotation);
}
//bool ATest2DCharacter::UpdateCharacter_Validate() { return true; }

void ATest2DCharacter::UpdateAnimation()
{
	if (!HasAuthority())
		return;

	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

	// Are we moving or standing still?
	DesiredAnimation = IdleAnimation;
	if (moveInput)
		DesiredAnimation = RunningAnimation;
	if (falling)
		DesiredAnimation = FallingAnimation;
	if (wallSliding)
		DesiredAnimation = WallSlidingAnimation;
	if (jumping)
		DesiredAnimation = JumpingAnimation;
	if (AttributeComponent != nullptr && AttributeComponent->IsValidLowLevel() && !AttributeComponent->isAlive())
		DesiredAnimation = DeathAnimation;
	if (SkillHandler != nullptr && SkillHandler->IsValidLowLevel() && SkillHandler->getCurrSkillAnim())
		DesiredAnimation = SkillHandler->getCurrSkillAnim();

	if (GetSprite()->GetFlipbook() != DesiredAnimation)
		GetSprite()->SetFlipbook(DesiredAnimation);
}
void ATest2DCharacter::UpdateRPC_Implementation(FRotator ARot, FVector VLook, UPaperFlipbook * newAnimation)
{
	actorRotation = ARot;
	SetActorRotation(ARot);

	lookDir = VLook;
	SkillHandler->ToTargetDir = VLook;

	DesiredAnimation = newAnimation;
	GetSprite()->SetFlipbook(newAnimation);
}
bool ATest2DCharacter::UpdateRPC_Validate(FRotator ARot, FVector VLook, UPaperFlipbook * newAnimation)
{
	return true;
}
void ATest2DCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATest2DCharacter, actorRotation);
	DOREPLIFETIME(ATest2DCharacter, moveInput);
	DOREPLIFETIME(ATest2DCharacter, falling);
	DOREPLIFETIME(ATest2DCharacter, wallSliding);
	DOREPLIFETIME(ATest2DCharacter, jumping);
	DOREPLIFETIME(ATest2DCharacter, wantsToJump);
	DOREPLIFETIME(ATest2DCharacter, moveRightDir);
	DOREPLIFETIME(ATest2DCharacter, moveUpDir);
	DOREPLIFETIME(ATest2DCharacter, lookDir);
	DOREPLIFETIME(ATest2DCharacter, DesiredAnimation);
}