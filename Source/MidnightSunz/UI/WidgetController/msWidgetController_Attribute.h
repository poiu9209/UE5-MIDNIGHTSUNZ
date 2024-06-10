// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "msWidgetController.h"
#include "GameplayTagContainer.h"
#include "msWidgetController_Attribute.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAttributeInfoSignature, const FGameplayTag&, GameplayTag, float, Value);

/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class MIDNIGHTSUNZ_API UmsWidgetController_Attribute : public UmsWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;
};
