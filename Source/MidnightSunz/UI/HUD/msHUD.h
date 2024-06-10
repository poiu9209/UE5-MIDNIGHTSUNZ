// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "msHUD.generated.h"

class UmsUserWidget;
class UmsWidgetController_Attribute;
class UmsWidgetController_Overlay;
class UmsWidgetController_Buff;
class AmsPlayerState;
class AmsPlayerController;
class UmsAttributeSet;
class UmsAbilitySystemComponent;
struct FWidgetControllerParams;

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API AmsHUD : public AHUD
{
	GENERATED_BODY()

public:
	UmsWidgetController_Overlay* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	UmsWidgetController_Attribute* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);
	UmsWidgetController_Buff* GetBuffWidgetController(const FWidgetControllerParams& WCParams);

	void InitOverlay(AmsPlayerController* PC, AmsPlayerState* PS, UmsAbilitySystemComponent* ASC, UmsAttributeSet* AS);

	void SetVisibleHUD(const bool bInShow);

	UmsUserWidget* GetOverlayWidget() const;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UmsUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UmsUserWidget> OverlayWidget;

	UPROPERTY()
	TObjectPtr<UmsWidgetController_Overlay> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UmsWidgetController_Overlay> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UmsWidgetController_Attribute> AttributeWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UmsWidgetController_Attribute> AttrubuteWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UmsWidgetController_Buff> BuffWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UmsWidgetController_Buff> BuffWidgetControllerClass;

};
