// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "msPlayerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UmsPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class MIDNIGHTSUNZ_API ImsPlayerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void StartInteraction();

	UFUNCTION(BlueprintImplementableEvent)
	void CompleteInteraction();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Revive();
};
