// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "MidnightSunz/msGameplayEffectContext.h"
#include "msProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class MIDNIGHTSUNZ_API AmsProjectile : public AActor
{
	GENERATED_BODY()

public:
	AmsProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	virtual void OnHit();
	bool IsValidOverlap(AActor* OtherActor);
	void ExecuteImpactCue();

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	FDamageEffectParams DamageEffectParams;;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY()
	TObjectPtr<USceneComponent> HomingTargetSceneComponnt;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag ImpactCueTag;

	UPROPERTY(EditAnywhere)
	float LifeSpan = 2.f;

	bool bHit = false;
};
