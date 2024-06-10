// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "msArtifactInterface.generated.h"

UINTERFACE(MinimalAPI)
class UmsArtifactInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class MIDNIGHTSUNZ_API ImsArtifactInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TriggerKillEnemyArtifact(const FVector& TargetLocation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TriggerHitEnemyArtifact(AActor* TargetActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TriggerNormalAttackArtifact();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetBaseDamage();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetProbability(const FGameplayTag& InArtifactTag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CheckProbability(const FGameplayTag& InArtifactTag);
};
