// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "msEffectActor.generated.h"

class UGameplayEffect;
class USoundBase;
class UNiagaraSystem;

UCLASS()
class MIDNIGHTSUNZ_API AmsEffectActor : public AActor
{
	GENERATED_BODY()

public:
	AmsEffectActor();

protected:
	virtual void Destroyed() override;

	UFUNCTION(BlueprintCallable)
	virtual bool ShouldApplyEffectToTarget(AActor* TargetActor) const;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, bool bDestroy = true);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EffectActor")
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EffectActor")
	TObjectPtr<USoundBase> PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EffectActor")
	TObjectPtr<UNiagaraSystem> PickupEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EffectActor")
	bool bApplyEffectsToEnemies = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EffectActor")
	float ActorLevel = 1.f;

};
