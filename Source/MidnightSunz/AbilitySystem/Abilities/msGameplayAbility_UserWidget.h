// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "msGameplayAbility.h"
#include "msGameplayAbility_UserWidget.generated.h"

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API UmsGameplayAbility_UserWidget : public UmsGameplayAbility
{
	GENERATED_BODY()

public:
	UmsGameplayAbility_UserWidget();
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

};
