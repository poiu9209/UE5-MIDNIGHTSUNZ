// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "msEffectActor.h"
#include "msEffectActor_HealPack.generated.h"

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API AmsEffectActor_HealPack : public AmsEffectActor
{
	GENERATED_BODY()

protected:
	virtual bool ShouldApplyEffectToTarget(AActor* TargetActor) const override;

};
