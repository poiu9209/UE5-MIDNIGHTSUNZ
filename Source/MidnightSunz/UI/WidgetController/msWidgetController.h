// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "msWidgetController.generated.h"

class AmsPlayerState;
class UmsAttributeSet;
class AmsPlayerController;
class UmsAbilitySystemComponent;
class UmsUserWidget;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams() {}

	FWidgetControllerParams(AmsPlayerController* PC, AmsPlayerState* PS, UmsAbilitySystemComponent* ASC, UmsAttributeSet* AS)
		: PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AmsPlayerController> PlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AmsPlayerState> PlayerState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UmsAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UmsAttributeSet> AttributeSet;
};

/**
 *
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FwWidgetControllerDelegateSignature);

UCLASS()
class MIDNIGHTSUNZ_API UmsWidgetController : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);

	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues();
	virtual void BindCallbacksToDependencies();

	UFUNCTION(BlueprintCallable)
	void BraodcastToggleShowWidget();

	UFUNCTION(BlueprintCallable)
	UmsUserWidget* GetOverlayWidget();

public:
	UPROPERTY(BlueprintAssignable)
	FwWidgetControllerDelegateSignature ToggleShowWidgetDelegate;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<AmsPlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<AmsPlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UmsAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UmsAttributeSet> AttributeSet;
};
