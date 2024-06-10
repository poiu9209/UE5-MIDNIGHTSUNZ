// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "msWidgetController.h"
#include "msWidgetController_Buff.generated.h"

class UmsBuffInfo;
/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class MIDNIGHTSUNZ_API UmsWidgetController_Buff : public UmsWidgetController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WidgetData")
	TObjectPtr<UmsBuffInfo> BuffInfo;

};
