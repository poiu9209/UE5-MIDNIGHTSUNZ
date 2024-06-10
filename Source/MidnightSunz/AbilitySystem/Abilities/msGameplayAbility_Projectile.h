// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "msGameplayAbility_Damage.h"
#include "msGameplayAbility_Projectile.generated.h"

class AmsProjectile;

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API UmsGameplayAbility_Projectile : public UmsGameplayAbility_Damage
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch = false, float PitchOverrdie = 0.f, AActor* HomingTarget = nullptr);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AmsProjectile> ProjectileClass;
};
