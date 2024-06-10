// Fill out your copyright notice in the Description page of Project Settings.


#include "msAbilitySystemGlobals.h"
#include "MidnightSunz/msGameplayEffectContext.h"

FGameplayEffectContext* UmsAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FmsGameplayEffectContext();
}
