// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "msCheatManager.generated.h"

struct FGameplayTag;
/**
 *
 */
UCLASS(Within = PlayerController)
class MIDNIGHTSUNZ_API UmsCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec)
	void AddArtifact(const FGameplayTag& InTag);

	UFUNCTION(Exec)
	void AddAllArtifact();

	UFUNCTION(Exec)
	void AddSkillLevel();

	UFUNCTION(Exec)
	void ProceedObjectiveComplete();

	UFUNCTION(Exec)
	void Suicide();

	UFUNCTION(Exec)
	void Revive();
};
