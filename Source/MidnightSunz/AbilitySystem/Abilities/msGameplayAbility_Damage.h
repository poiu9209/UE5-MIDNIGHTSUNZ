// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "msGameplayAbility.h"
#include "MidnightSunz/Interface/msCombatInterface.h"
#include "MidnightSunz/msGameplayEffectContext.h"
#include "msGameplayAbility_Damage.generated.h"

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API UmsGameplayAbility_Damage : public UmsGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefault(AActor* TargetActor = nullptr, FVector InRadialDamageOrigin = FVector::ZeroVector) const;

	UFUNCTION(BlueprintCallable)
	float GetDamage(int32 InLevel) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	bool bIsRadialDamage = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float RadialDamageInnerRadius = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float RadialDamageOuterRadius = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage|Debuff")
	float DebuffChance = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage|Debuff")
	float DebuffDamage = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage|Debuff")
	float DebuffFrequency = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage|Debuff")
	float DebuffDuration = 5.f;

	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;
};
