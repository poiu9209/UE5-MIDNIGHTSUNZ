// Fill out your copyright notice in the Description page of Project Settings.


#include "msHUD.h"
#include "Blueprint/UserWidget.h"
#include "MidnightSunz/UI/Widgets/msUserWidget.h"
#include "MidnightSunz/UI/WidgetController/msWidgetController_Overlay.h"
#include "MidnightSunz/UI/WidgetController/msWidgetController_Attribute.h"
#include "MidnightSunz/UI/WidgetController/msWidgetController_Buff.h"
#include "MidnightSunz/Player/msPlayerController.h"

UmsWidgetController_Overlay* AmsHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (!OverlayWidgetController)
	{
		OverlayWidgetController = NewObject<UmsWidgetController_Overlay>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();
	}

	return OverlayWidgetController;
}

UmsWidgetController_Attribute* AmsHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (!AttributeWidgetController)
	{
		AttributeWidgetController = NewObject<UmsWidgetController_Attribute>(this, AttrubuteWidgetControllerClass);
		AttributeWidgetController->SetWidgetControllerParams(WCParams);
		AttributeWidgetController->BindCallbacksToDependencies();
	}

	return AttributeWidgetController;
}

UmsWidgetController_Buff* AmsHUD::GetBuffWidgetController(const FWidgetControllerParams& WCParams)
{
	if (!BuffWidgetController)
	{
		BuffWidgetController = NewObject<UmsWidgetController_Buff>(this, BuffWidgetControllerClass);
		BuffWidgetController->SetWidgetControllerParams(WCParams);
		BuffWidgetController->BindCallbacksToDependencies();
	}
	return BuffWidgetController;
}

void AmsHUD::InitOverlay(AmsPlayerController* PC, AmsPlayerState* PS, UmsAbilitySystemComponent* ASC, UmsAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class unititialized, pleas fill out BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class unititialized, pleas fill out BP_AuraHUD"));

	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UmsUserWidget>(Widget);

	const FWidgetControllerParams Params(PC, PS, ASC, AS);
	UmsWidgetController_Overlay* WidgetController = GetOverlayWidgetController(Params);

	OverlayWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValues();
	Widget->AddToViewport();

	if (PC->IsDisableGameplay())
	{
		SetVisibleHUD(false);
	}
}

void AmsHUD::SetVisibleHUD(const bool bInShow)
{
	if (OverlayWidget)
	{
		OverlayWidget->SetVisibility(bInShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

UmsUserWidget* AmsHUD::GetOverlayWidget() const
{
	return OverlayWidget;
}
