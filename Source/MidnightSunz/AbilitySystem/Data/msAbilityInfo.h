// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "msAbilityInfo.generated.h"

class UGameplayEffect;
class UmsArtifact;

USTRUCT(BlueprintType)
struct FAbilityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag CooldownTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<const UTexture2D> Icon = nullptr;
};

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API UmsAbilityInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityInformation")
	TArray<FAbilityInfo> AbilityInformation;

	FAbilityInfo FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound = false) const;
};


USTRUCT(BlueprintType)
struct FBuffInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag BuffTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<const UTexture2D> Icon = nullptr;
};

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API UmsBuffInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuffInformation")
	TArray<FBuffInfo> BuffInformation;
};

UCLASS()
class MIDNIGHTSUNZ_API UmsIconInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "IconInformation")
	TMap<FGameplayTag, TSoftObjectPtr<const UTexture2D>> IconInformation;
};


USTRUCT(BlueprintType)
struct FArtifactInfo
{
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, meta = (Displayname = "Description"))
	FString Desc;
#endif // WITH_EDITORONLY_DATA

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag Tag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> ArtifactEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UmsArtifact> ArtifactObj = nullptr;
};

USTRUCT(BlueprintType)
struct FArtifactData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FArtifactInfo> Tier1Artifacts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FArtifactInfo> Tier2Artifacts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FArtifactInfo> Tier3Artifacts;
};

UCLASS()
class MIDNIGHTSUNZ_API UmsArtifactInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Artifact")
	FArtifactData ArtifactData;

};