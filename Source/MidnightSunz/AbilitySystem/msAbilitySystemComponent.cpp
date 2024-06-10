// Fill out your copyright notice in the Description page of Project Settings.


#include "msAbilitySystemComponent.h"
#include "Abilities/msGameplayAbility.h"
#include "MidnightSunz/msLogChannels.h"
#include "MidnightSunz/Player/msPlayerState.h"
#include "MidnightSunz/Character/msCharacter.h"
#include "MidnightSunz/msGameplayTags.h"

void UmsAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const auto& AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1.f);
		if (const UmsGameplayAbility* Ability = Cast<UmsGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(Ability->InputTag);
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast(this);
}

void UmsAbilitySystemComponent::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
		}
	}
}

void UmsAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UmsAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UmsAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	// 어빌리티를 반복처리하는 동안 어빌리티 시스템 컴포넌트에서 어빌리티를 제거하지 못하게 하는 데 사용됩니다
	FScopedAbilityListLock AbilityScopeLock(*this);
	for (const auto& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(Logms, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UmsAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (const auto& Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}

	return FGameplayTag();
}

FGameplayTag UmsAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const auto& Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}

	return FGameplayTag();
}

FGameplayAbilitySpec* UmsAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(AbilityTag))
			{
				return &AbilitySpec;
			}
		}
	}
	return nullptr;
}

FGameplayAbilitySpec* UmsAbilitySystemComponent::GetSpecFromInputTag(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			return &AbilitySpec;
		}
	}
	return nullptr;
}

FGameplayAbilitySpec* UmsAbilitySystemComponent::LevelUpAbility(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		Spec->Level += 1;
		MarkAbilitySpecDirty(*Spec);
		ClientUpdateAbilityStatus(AbilityTag, Spec->Level);
		return Spec;
	}
	return nullptr;
}

void UmsAbilitySystemComponent::ApplySavedAbilityLevel(FPlayerSaveData& AbilityLevel)
{
	for (auto& LevelMap : AbilityLevel.SkillLevel)
	{
		if (FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(LevelMap.Key))
		{
			Spec->Level = LevelMap.Value;
			MarkAbilitySpecDirty(*Spec);
			ClientUpdateAbilityStatus(LevelMap.Key, Spec->Level);
		}
	}
}

void UmsAbilitySystemComponent::GetSkillAbilityLevel(FPlayerSaveData& OutData)
{
	OutData.SkillLevel.Empty();
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (const auto& Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Skill"))))
			{
				OutData.SkillLevel.Add(Tag, AbilitySpec.Level);
			}
		}
	}
}

void UmsAbilitySystemComponent::ResetSkillCool()
{
	TArray<UGameplayEffect*> Cooldowns;
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (const auto& Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Skill"))))
			{
				Cooldowns.AddUnique(AbilitySpec.Ability.Get()->GetCooldownGameplayEffect());
				continue;
			}
		}
	}

	FGameplayTagContainer TagContainer;
	for (UGameplayEffect* GE : Cooldowns)
	{
		if (GE)
		{
			TagContainer.AppendTags(GE->InheritableOwnedTagsContainer.CombinedTags);
		}
	}

	if (TagContainer.IsValid())
	{
		RemoveActiveEffectsWithGrantedTags(TagContainer);
	}
}

void UmsAbilitySystemComponent::CheatAddSkillLevel()
{
#if WITH_EDITOR
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (const auto& Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Skill"))))
			{
				AbilitySpec.Level += 1;
				MarkAbilitySpecDirty(AbilitySpec);
				ClientUpdateAbilityStatus(Tag, AbilitySpec.Level);
			}
		}
	}
#endif
}

void UmsAbilitySystemComponent::GetAbilityEvolutionText(FGameplayAbilitySpec* InSpec, int32 InLevel, FText& OutText)
{
	if (InSpec)
	{
		if (UmsGameplayAbility* Ability = Cast<UmsGameplayAbility>(InSpec->Ability))
		{
			Ability->GetEvolutionText(InLevel, OutText);
		}
	}
}

void UmsAbilitySystemComponent::GetAbilityDescText(FGameplayAbilitySpec* InSpec, int32 InLevel, FText& OutText)
{
	if (InSpec)
	{
		if (UmsGameplayAbility* Ability = Cast<UmsGameplayAbility>(InSpec->Ability))
		{
			Ability->GetDescText(InLevel, OutText);
		}
	}
}

void UmsAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast(this);
	}
}

void UmsAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& InTag, int32 InLevel)
{
	AbilityStatusChangedDelegate.Broadcast(InTag, InLevel);
}
