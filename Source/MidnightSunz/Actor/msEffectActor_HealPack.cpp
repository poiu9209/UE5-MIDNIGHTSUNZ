// Fill out your copyright notice in the Description page of Project Settings.


#include "msEffectActor_HealPack.h"
#include "MidnightSunz/Interface/msCombatInterface.h"

bool AmsEffectActor_HealPack::ShouldApplyEffectToTarget(AActor* TargetActor) const
{
	if (!Super::ShouldApplyEffectToTarget(TargetActor))
	{
		return false;
	}

	return !Cast<ImsCombatInterface>(TargetActor)->IsFullHealth();
}
