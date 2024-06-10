
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "msGameplayAbility_Damage.h"
#include "msGameplayAbility_DragonBullet.generated.h"

class AmsProjectile_Penetration;
struct FGameplayTag;

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API UmsGameplayAbility_DragonBullet : public UmsGameplayAbility_Damage
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SpawnDragonBullets(const FGameplayTag& SocketTag);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dragon Bullet")
	int32 NumDragonBullets = 12;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Spread = 90.f;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AmsProjectile_Penetration> DragonBulletClass;
};
