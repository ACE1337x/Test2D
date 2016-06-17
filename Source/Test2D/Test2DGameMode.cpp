// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "Test2D.h"
#include "Test2DGameMode.h"
#include "Test2DCharacter.h"
#include "Kismet/GameplayStatics.h"

ATest2DGameMode::ATest2DGameMode()
{
	// set default pawn class to our character
	DefaultPawnClass = ATest2DCharacter::StaticClass();
}

APlayerController * ATest2DGameMode::Login(UPlayer * NewPlayer, ENetRole InRemoteRole, const FString & Portal, const FString & Options, const TSharedPtr<const FUniqueNetId>& UniqueId, FString & ErrorMessage)
{
	APlayerController * ret = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	playerName = UGameplayStatics::ParseOption(Options, FString("CharacterName"));
	LoggedIn(playerName, ret);
	return 	ret;
}

void ATest2DGameMode::LoggedIn_Implementation(const FString &name, APlayerController* controller)
{

}
