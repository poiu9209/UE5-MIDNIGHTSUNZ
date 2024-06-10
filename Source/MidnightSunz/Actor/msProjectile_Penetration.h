// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "msProjectile.h"
#include "msProjectile_Penetration.generated.h"

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API AmsProjectile_Penetration : public AmsProjectile
{
	GENERATED_BODY()

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnHit() override;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> OverlapActors;

};
