// Fill out your copyright notice in the Description page of Project Settings.


#include "msProjectile_Penetration.h"
#include "Components/AudioComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "MidnightSunz/AbilitySystem/msAbilitySystemLibrary.h"

void AmsProjectile_Penetration::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor))
	{
		return;
	}

	if (OverlapActors.Contains(OtherActor))
	{
		return;
	}

	ExecuteImpactCue();

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UmsAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
			OverlapActors.AddUnique(OtherActor);
		}
	}
}

void AmsProjectile_Penetration::OnHit()
{
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}

	bHit = true;
}
