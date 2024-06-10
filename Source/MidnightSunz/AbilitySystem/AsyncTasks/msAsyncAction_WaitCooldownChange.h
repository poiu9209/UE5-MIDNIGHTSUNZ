// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GameplayTagContainer.h"
#include "ActiveGameplayEffectHandle.h"
#include "msAsyncAction_WaitCooldownChange.generated.h"

class UAbilitySystemComponent;
struct FGameplayEffectSpec;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangeSignature, float, TimeRemaining);

/**
 *
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncTask"))
class MIDNIGHTSUNZ_API UmsAsyncAction_WaitCooldownChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownStart;

	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownEnd;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UmsAsyncAction_WaitCooldownChange* WaitForCooldownChange(UAbilitySystemComponent* InASC, const FGameplayTag& InCooldownTag, const bool bInCountChange = false);

	UFUNCTION(BlueprintCallable)
	void EndTask();

	UFUNCTION(BlueprintCallable)
	float GetTimeRemaining() const;

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	FGameplayTag CooldownTag;

	bool bCountChange = false;

	void CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount);
	void OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle);
private:
	void HandleCooldownStart();
};
