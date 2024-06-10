// Fill out your copyright notice in the Description page of Project Settings.


#include "msWidgetController.h"
#include "MidnightSunz/Player/msPlayerController.h"
#include "MidnightSunz/Player/msPlayerState.h"
#include "MidnightSunz/AbilitySystem/msAbilitySystemComponent.h"
#include "MidnightSunz/AbilitySystem/Attributes/msAttributeSet.h"
#include "MidnightSunz/UI/HUD/msHUD.h"

void UmsWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UmsWidgetController::BroadcastInitialValues()
{
}

void UmsWidgetController::BindCallbacksToDependencies()
{
}

void UmsWidgetController::BraodcastToggleShowWidget()
{
	ToggleShowWidgetDelegate.Broadcast();
}

UmsUserWidget* UmsWidgetController::GetOverlayWidget()
{
	if (PlayerController)
	{
		if (AmsHUD* hud = Cast<AmsHUD>(PlayerController->GetHUD()))
		{
			return hud->GetOverlayWidget();
		}
	}
	return nullptr;
}
