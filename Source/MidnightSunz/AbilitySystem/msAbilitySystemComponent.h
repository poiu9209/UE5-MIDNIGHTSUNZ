// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MidnightSunz/GameModes/msGameTypes.h"
#include "msAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FAbilitiesGivenSignature, UmsAbilitySystemComponent*);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FAbilityStatusChangedSignature, const FGameplayTag& /*AbilityTag*/, int32 /*Level*/);

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API UmsAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);

	FAbilitiesGivenSignature AbilitiesGivenDelegate;
	FAbilityStatusChangedSignature AbilityStatusChangedDelegate;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ForEachAbility(const FForEachAbility& Delegate);

	bool IsStartupAbilitiesGiven() const { return bStartupAbilitiesGiven; }

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayAbilitySpec* GetSpecFromInputTag(const FGameplayTag& InputTag);

	FGameplayAbilitySpec* LevelUpAbility(const FGameplayTag& AbilityTag);
	void ApplySavedAbilityLevel(FPlayerSaveData& AbilityLevel);

	void GetSkillAbilityLevel(FPlayerSaveData& OutData);

	UFUNCTION(BlueprintCallable)
	void ResetSkillCool();

	void CheatAddSkillLevel();

	void GetAbilityEvolutionText(FGameplayAbilitySpec* InSpec, int32 InLevel, FText& OutText);
	void GetAbilityDescText(FGameplayAbilitySpec* InSpec, int32 InLevel, FText& OutText);

protected:
	virtual void OnRep_ActivateAbilities() override;

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& InTag, int32 InLevel);
private:
	bool bStartupAbilitiesGiven = false;

};
