// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "msEffectActorPickups.generated.h"

class AmsEffectActor;

UCLASS()
class MIDNIGHTSUNZ_API AmsEffectActorPickups : public AActor
{
	GENERATED_BODY()

public:
	AmsEffectActorPickups();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void StartSpawnTimer(AActor* DestroyedActor);

	UFUNCTION()
	void SpawnTimerFinished();

	void SpawnEffectActor();

private:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AmsEffectActor>> EffectActorClasses;

	UPROPERTY()
	TObjectPtr<AmsEffectActor> SpawnedActor;

	FTimerHandle SpawnTimerHandle;

	UPROPERTY(EditAnywhere, meta = (min = 0.1))
	float SpawnTimerMin;

	UPROPERTY(EditAnywhere, meta = (min = 0.1))
	float SpawnTimerMax;
};
