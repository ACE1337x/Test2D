// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "GameFramework/GameMode.h"
#include "Test2DGameMode.generated.h"

// The GameMode defines the game being played. It governs the game rules, scoring, what actors
// are allowed to exist in this game type, and who may enter the game.
//
// This game mode just sets the default pawn to be the MyCharacter asset, which is a subclass of Test2DCharacter

UCLASS(minimalapi)
class ATest2DGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ATest2DGameMode();
	FString playerName;
	virtual APlayerController* Login(class UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const TSharedPtr<const FUniqueNetId>& UniqueId, FString& ErrorMessage) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Skill")
		void LoggedIn(const FString &name, APlayerController* controller);
};
