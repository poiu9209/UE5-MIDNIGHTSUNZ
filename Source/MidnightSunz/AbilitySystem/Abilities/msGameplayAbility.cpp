// Fill out your copyright notice in the Description page of Project Settings.


#include "msGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "MidnightSunz/msGameplayTags.h"

void UmsGameplayAbility::CommitCooldown()
{
	if (auto CooldownEffect = GetCooldownGameplayEffect())
	{
		const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGameplayEffectClass, GetAbilityLevel());

		const float BaseCooldown = Cooldown.GetValueAtLevel(GetAbilityLevel());
		const bool HasBuff = GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(FmsGameplayTags::Get().Buff_ReduceCooldown);

		float BuffMagnitude = HasBuff ? 0.5f : 1.f;
		float SetByCallerCooldown = BaseCooldown * BuffMagnitude;

		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FmsGameplayTags::Get().SetByCaller_Cooldown, SetByCallerCooldown);
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle);
	}
}

float UmsGameplayAbility::GetCooldown(int32 InLevel)
{
	return Cooldown.GetValueAtLevel(InLevel);
}
