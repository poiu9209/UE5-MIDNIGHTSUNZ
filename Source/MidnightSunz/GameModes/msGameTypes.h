#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "msGameTypes.generated.h"

UENUM(BlueprintType)
enum class EPointType : uint8
{
	AbilityLevel UMETA(DisplayName = "Ability Level"),
	Artifact UMETA(DisplayName = "Artifact")
};

USTRUCT(BlueprintType)
struct FEvolutionOption
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FGameplayTag> EvolutionTags;
};

USTRUCT(BlueprintType)
struct FPlayerSaveData
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FGameplayTag, uint8> SkillLevel;

	UPROPERTY()
	TMap<FGameplayTag, uint8> ArtifactLevel;

	UPROPERTY()
	TArray<FEvolutionOption> Evolutions;

	void Clear()
	{
		SkillLevel.Empty();
		ArtifactLevel.Empty();
		Evolutions.Empty();
	}
};
