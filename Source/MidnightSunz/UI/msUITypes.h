#pragma once

#include "GameplayTagContainer.h"
#include "msUITypes.generated.h"

USTRUCT(BlueprintType)
struct FDamageTextParams
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag DamageType;

	UPROPERTY(BlueprintReadOnly)
	int32 DamageAmount = 0;

	UPROPERTY(BlueprintReadOnly)
	bool bIsCriticalHit = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsBlockedDamage = false;

	FDamageTextParams() {}
	FDamageTextParams(const FGameplayTag& InDamageType, int32 InDamageAmount, bool bInIsCriticalHit)
		: DamageType(InDamageType), DamageAmount(InDamageAmount), bIsCriticalHit(bInIsCriticalHit)
	{}
};