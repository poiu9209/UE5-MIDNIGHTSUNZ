// Fill out your copyright notice in the Description page of Project Settings.


#include "msEffectActorPickups.h"
#include "msEffectActor.h"

AmsEffectActorPickups::AmsEffectActorPickups()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void AmsEffectActorPickups::BeginPlay()
{
	Super::BeginPlay();
	StartSpawnTimer(nullptr);
}

void AmsEffectActorPickups::StartSpawnTimer(AActor* DestroyedActor)
{
	const float SpawnTime = FMath::FRandRange(SpawnTimerMin, SpawnTimerMax);
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ThisClass::SpawnTimerFinished, SpawnTime);
}

void AmsEffectActorPickups::SpawnTimerFinished()
{
	if (HasAuthority())
	{
		SpawnEffectActor();
	}
}

void AmsEffectActorPickups::SpawnEffectActor()
{
	int32 NumActorClasses = EffectActorClasses.Num();
	if (NumActorClasses > 0)
	{
		int32 Selection = FMath::RandRange(0, NumActorClasses - 1);
		SpawnedActor = GetWorld()->SpawnActor<AmsEffectActor>(EffectActorClasses[Selection], GetActorTransform());

		if (HasAuthority() && SpawnedActor)
		{
			SpawnedActor->OnDestroyed.AddDynamic(this, &ThisClass::StartSpawnTimer);
		}
	}
}
