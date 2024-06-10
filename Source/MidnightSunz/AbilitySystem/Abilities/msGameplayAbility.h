// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "msGameplayAbility.generated.h"

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API UmsGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FGameplayTag InputTag;

	UFUNCTION(BlueprintCallable)
	void CommitCooldown();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void GetEvolutionText(int32 InLevel, FText& OutText);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void GetDescText(int32 InLevel, FText& OutText);

	UFUNCTION(BlueprintPure)
	float GetCooldown(int32 InLevel);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cooldown")
	FScalableFloat Cooldown;

};
