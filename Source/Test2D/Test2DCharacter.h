// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PaperCharacter.h"
#include "SkillHandlerComponent.h"
#include "Test2DCharacter.generated.h"

// This class is the default character for Test2D, and it is responsible for all
// physical interaction between the player and the world.
//
//   The capsule component (inherited from ACharacter) handles collision with the world
//   The CharacterMovementComponent (inherited from ACharacter) handles movement of the collision capsule
//   The Sprite component (inherited from APaperCharacter) handles the visuals

class UTextRenderComponent;

UCLASS(config = Game)
class ATest2DCharacter : public APaperCharacter
{
	GENERATED_BODY()

		///** Side view camera */
		//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
		//class UCameraComponent* SideViewCameraComponent;

		///** Camera boom positioning the camera beside the character */
		//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		//class USpringArmComponent* CameraBoom;

		UTextRenderComponent* TextComponent;
	virtual void Tick(float DeltaSeconds) override;
private:
protected:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
		bool wantsToJump = 0;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
		float moveRightDir = 0;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
		float moveUpDir = 0;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Movement")
		bool moveInput;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Animations)
		bool falling;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Animations)
		bool jumping;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Animations)
		FRotator actorRotation;
	UPROPERTY(Replicated, BlueprintReadOnly, Category = Animations)
		UPaperFlipbook* DesiredAnimation;

	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* RunningAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* IdleAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* FallingAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* WallSlidingAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* JumpingAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* CurrSkillAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* DeathAnimation;

	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();

	/** Called for side to side input */
	UFUNCTION(/*Server, WithValidation, Reliable,*/ BlueprintCallable, Category = "Player Functions")
		void UpdateCharacter();

	UFUNCTION(Server, WithValidation, Reliable)
		virtual void UpdateRPC(FRotator ARot, FVector VLook, UPaperFlipbook * newAnimation);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Player Functions")
		void Landed(const FHitResult &Hit) override;

	//UPROPERTY(BlueprintAssignable, Category = "Landed")
	//	FLandedDelegate OnLanded;

	/** Handle touch inputs. */
//	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Handle touch stop event. */
//	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	ATest2DCharacter();

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Wallsliding")
		bool wallSliding;

	UPROPERTY(Category = Test2DCharacter,/* Replicated,*/ VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USkillHandlerComponent * SkillHandler;
	UPROPERTY(Category = Test2DCharacter, /* Replicated,*/ VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UAttributeComponent * AttributeComponent;
	UPROPERTY(Category = Test2DCharacter, /* Replicated,*/ VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UInventoryComponent * InventoryComponent;
	UPROPERTY(Category = Test2DCharacter, /* Replicated,*/ VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UBoxComponent * wallSlideCollision;
	UPROPERTY(Category = Test2DCharacter, /* Replicated,*/ VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USceneComponent * SceneComponent;

	/** Returns SideViewCameraComponent subobject **/
	//FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	///** Returns CameraBoom subobject **/
	//FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Player Functions")
		void MoveRight(float Value);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Player Functions")
		void MoveUp(float Value);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Player Functions")
		void Jump();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Player Functions")
		void TryUseSkill(ESkillSlotTypes skillSlotToUse);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Direction)
		FVector lookDir;

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

};
