// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "msUserWidget.h"
#include "msReturnMainMenu.generated.h"

class UMultiplayerSessionsSubsystem;
class APlayerController;
class UButton;


/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API UmsReturnMainMenu : public UmsUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

	UFUNCTION()
	void ReturnButtonClicked();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_RetrunMenu;

	UPROPERTY()
	TObjectPtr<UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;
};
