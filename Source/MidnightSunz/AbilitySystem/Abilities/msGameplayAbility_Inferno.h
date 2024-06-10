// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "msGameplayAbility_Projectile.h"
#include "msGameplayAbility_Inferno.generated.h"

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API UmsGameplayAbility_Inferno : public UmsGameplayAbility_Projectile
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SpawnMeteors(const TArray<AActor*>& TargetLocations);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inferno")
	float InferoRadius = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inferno")
	int32 MaxNumProjectiles = 12;

	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float HomingAccelerationMin = 1600.f;

	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float HomingAccelerationMax = 3200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Inferno")
	bool bIsHomingProjectile = true;

	UPROPERTY(EditDefaultsOnly, Category = "Inferno")
	float SpawnRandPos = 0.f;

};
