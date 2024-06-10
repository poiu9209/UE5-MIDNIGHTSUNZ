// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "msGameplayAbility_Damage.h"
#include "msGameplayAbility_TeslaChain.generated.h"

class UGameplayEffect;
/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API UmsGameplayAbility_TeslaChain : public UmsGameplayAbility_Damage
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void StoreElectrocuteTargets();

	UFUNCTION(BlueprintImplementableEvent)
	void AddElectrocuteCue(AActor* Target);

	UFUNCTION(BlueprintImplementableEvent)
	void RemoveElectrocuteCue(AActor* Target);

	UFUNCTION()
	void AvataActorDied(AActor* DeadActor);

	UFUNCTION()
	void ElectrocuteTargetDied(AActor* DeadActor);

	UFUNCTION(BlueprintCallable)
	void ApplyElectrocuteEffect();

	UFUNCTION(BlueprintCallable)
	void ClearAllElectrocuteEffects();

	UFUNCTION(BlueprintCallable)
	void RemoveElectrocuteEffectFromTarget(AActor* Target);

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

	UFUNCTION()
	void ExpiredElectocute();

	UFUNCTION()
	void ApplyEletrocuteDamage();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tesla Chain")
	float ElectocuteRadius = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Tesla Chain")
	int32 NumElectrocute = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Tesla Chain")
	float ElectocuteDuration = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Tesla Chain")
	float DamageFrequency = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Tesla Chain")
	TSubclassOf<UGameplayEffect> ElectocuteEffect;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> DamageToTargets;

	TWeakObjectPtr<AActor> SourceActor;

private:
	FTimerHandle EndTimerHandle;

	FTimerHandle DamageTimerHandle;

};
