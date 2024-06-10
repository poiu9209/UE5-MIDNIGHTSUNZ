// Fill out your copyright notice in the Description page of Project Settings.


#include "msGameplayAbility_Projectile.h"
#include "MidnightSunz/Actor/msProjectile.h"
#include "MidnightSunz/Interface/msCombatInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"


void UmsGameplayAbility_Projectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UmsGameplayAbility_Projectile::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch /*= false*/, float PitchOverrdie/* = 0.f*/, AActor* HomingTarget /*= nullptr*/)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority())
	{
		return;
	}

	ImsCombatInterface* CombatInterface = Cast<ImsCombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface)
	{
		return;
	}

	const FVector SocketLocation = ImsCombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverrdie;
	}

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	//
	//DrawDebugLine(GetAvatarActorFromActorInfo()->GetWorld(), SocketLocation, ProjectileTargetLocation, FColor::Red, false, 2.f);
	//
	AmsProjectile* Projectile = GetWorld()->SpawnActorDeferred<AmsProjectile>(ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(), Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefault();

	Projectile->FinishSpawning(SpawnTransform);
}
