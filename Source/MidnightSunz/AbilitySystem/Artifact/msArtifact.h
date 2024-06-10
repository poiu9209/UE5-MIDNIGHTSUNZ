// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "msArtifact.generated.h"

/**
 *
 */

UCLASS(Blueprintable, BlueprintType)
class MIDNIGHTSUNZ_API UmsArtifact : public UObject
{
	GENERATED_BODY()

public:
	void InitArtifact(const FGameplayTag& InTag, const int32 InStack);
	void AddStack() { StackCount++; }

	float GetProbability() const;

	UFUNCTION(BlueprintCallable)
	bool CheckProbability() const;

protected:
	UPROPERTY(VisibleAnywhere)
	FGameplayTag ArtifactTag;

	UPROPERTY(BlueprintReadOnly)
	int32 StackCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseValue;
};

/**
* 적을 처치할때 발동하는 아티팩트
*/
UCLASS()
class MIDNIGHTSUNZ_API UmsArtifact_KillEnemy : public UmsArtifact
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void Trigger(AActor* OwnerActor, const FVector& TargetLocation);
};

/**
* 적에게 공격이 명중할때 발동하는 아티팩트
*/
UCLASS()
class MIDNIGHTSUNZ_API UmsArtifact_HitEnemy : public UmsArtifact
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void Trigger(AActor* OwnerActor, AActor* TargetActor);
};

/**
* 일반공격(좌클릭)이 발생할때 발동하는 아티팩트
*/
UCLASS()
class MIDNIGHTSUNZ_API UmsArtifact_NormalAttack : public UmsArtifact
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void Trigger(AActor* OwnerActor);
};
