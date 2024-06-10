// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "msAbilityTask_TargetUnderMouse.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCursorTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API UmsAbilityTask_TargetUnderMouse : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "TargetUnderMouse", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = true))
	static UmsAbilityTask_TargetUnderMouse* CreateTargetUnderMouse(UGameplayAbility* OwningAbility);

	UPROPERTY(BlueprintAssignable)
	FCursorTargetDataSignature ValidData;

private:
	virtual void Activate() override;

	void SendMouseCursorData();
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);

	UPROPERTY()
	TObjectPtr<APawn> CachedPawn;
};
