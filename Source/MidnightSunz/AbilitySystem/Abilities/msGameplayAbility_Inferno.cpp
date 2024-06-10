// Fill out your copyright notice in the Description page of Project Settings.


#include "msGameplayAbility_Inferno.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MidnightSunz/AbilitySystem/msAbilitySystemLibrary.h"
#include "MidnightSunz/Actor/msProjectile.h"
#include "MidnightSunz/Interface/msCombatInterface.h"

void UmsGameplayAbility_Inferno::SpawnMeteors(const TArray<AActor*>& TargetLocations)
{
	int32 SpawnNum = 0;
	for (const auto& Target : TargetLocations)
	{
		if (!UmsAbilitySystemLibrary::IsNotFriend(GetAvatarActorFromActorInfo(), Target))
		{
			continue;
		}

		if (SpawnNum > MaxNumProjectiles)
		{
			break;
		}

		FTransform SpawnTransform;
		const float RandPos = FMath::FRandRange(-SpawnRandPos, SpawnRandPos);
		FVector SpawnLoc = Target->GetActorLocation() + FVector(RandPos, RandPos, 1200.f);

		//DrawDebugLine(GetWorld(), SpawnLoc, Target->GetActorLocation(), FColor::Red, false, 30.f);
		FRotator Rotaion = (Target->GetActorLocation() - SpawnLoc).Rotation();
		SpawnTransform.SetLocation(SpawnLoc);
		SpawnTransform.SetRotation(Rotaion.Quaternion());

		AmsProjectile* Projectile = GetWorld()->SpawnActorDeferred<AmsProjectile>(ProjectileClass, SpawnTransform, GetAvatarActorFromActorInfo(), CurrentActorInfo->PlayerController->GetPawn(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefault(Target);

		if (Target && Target->Implements<UmsCombatInterface>())
		{
			Projectile->ProjectileMovement->HomingTargetComponent = Target->GetRootComponent();
		}

		Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMin, HomingAccelerationMax);
		Projectile->ProjectileMovement->bIsHomingProjectile = bIsHomingProjectile;

		Projectile->FinishSpawning(SpawnTransform);
	}
}
