// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "msGameplayAbility.h"
#include "msGameplayAbility_Summon.generated.h"

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API UmsGameplayAbility_Summon : public UmsGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Summoning")
	TArray<FVector> GetSpawnLocations() const;

	UFUNCTION(BlueprintCallable, Category = "Summoning")
	TSubclassOf<AActor> GetRandomSummonClass() const;

	UPROPERTY(EditAnywhere, Category = "Summoning")
	bool bDrawDebug = false;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	int32 NumSummon = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	TArray<TSubclassOf<AActor>> SummonClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Summoning", meta = (ClampMin = 50.0))
	float MinSpawnDistance = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	float MaxSpawnDistance = 250.f;

	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	float SpawnSpread = 90.f;
};
