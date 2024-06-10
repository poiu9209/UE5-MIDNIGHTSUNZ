// Fill out your copyright notice in the Description page of Project Settings.


#include "msGameplayAbility_DragonBullet.h"
#include "MidnightSunz/AbilitySystem/msAbilitySystemLibrary.h"
#include "MidnightSunz/Actor/msProjectile_Penetration.h"

void UmsGameplayAbility_DragonBullet::SpawnDragonBullets(const FGameplayTag& SocketTag)
{
	ImsCombatInterface* CombatInterface = Cast<ImsCombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface)
	{
		return;
	}

	const FVector SocketLocation = ImsCombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const int32 NumOfBullet = FMath::Min(NumDragonBullets, GetAbilityLevel());
	TArray<FRotator> Rotators = UmsAbilitySystemLibrary::EvenlySpaceRotators(Forward, FVector::UpVector, Spread, NumOfBullet);

	for (const auto& Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotator.Quaternion());

		AmsProjectile_Penetration* DrangonBullet = GetWorld()->SpawnActorDeferred<AmsProjectile_Penetration>(
			DragonBulletClass,
			SpawnTransform,
			GetAvatarActorFromActorInfo(),
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		DrangonBullet->SetOwner(GetAvatarActorFromActorInfo());
		DrangonBullet->DamageEffectParams = MakeDamageEffectParamsFromClassDefault();
		DrangonBullet->FinishSpawning(SpawnTransform);
	}
}
