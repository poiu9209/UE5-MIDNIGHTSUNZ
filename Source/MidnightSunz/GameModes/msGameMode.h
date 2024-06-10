// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "msGameTypes.h"
#include "ScalableFloat.h"
#include "MidnightSunz/AbilitySystem/Data/msAbilityInfo.h"
#include "msGameMode.generated.h"

class UmsCharacterClassInfo;
class UmsArtifactInfo;
class UAbilitySystemComponent;
class AmsGameState;

struct FGameplayTag;

namespace msStageState
{
	extern MIDNIGHTSUNZ_API const FName None;
	extern MIDNIGHTSUNZ_API const FName StageStart;
	extern MIDNIGHTSUNZ_API const FName WaitNextStage;
	extern MIDNIGHTSUNZ_API const FName GameOver;
}

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FObjectiveComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStageStatechange, FName, StageName);

UCLASS()
class MIDNIGHTSUNZ_API AmsGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void InitGameState() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName = TEXT("")) override;
	virtual void HandleMatchHasStarted() override;

	virtual bool AllowCheats(APlayerController* P);

	void HandleCharacterDeath(ACharacter* DeadCharacter);

	int32 GetObjectiveCount() const;

	UPROPERTY(BlueprintAssignable)
	FObjectiveComplete ObjectiveCompleteDelegate;

	UPROPERTY(BlueprintAssignable)
	FStageStatechange StageChangeDelegate;

	TSubclassOf<UmsArtifact> EvolutionArtifact(const FGameplayTag& InTag, UAbilitySystemComponent* ASC, uint8 InStack = 1) const;

	void GetRandomEvolutionTag(TArray<FGameplayTag>& OutTag);

	UFUNCTION(BlueprintCallable)
	void NextStageStart();

	void ObjectiveComplete();

	void RequestRespawn(ACharacter* InCharacter, AController* InController);

protected:
	void SetStageState(FName NewState);
	void OnStageStateSet();

private:
	TArray<FGameplayTag>& GetArtifactListByRandomTier();

	void CacheArtifactList() const;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Character Defaults")
	TObjectPtr<UmsCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(Transient)
	FName StageState = msStageState::None;

	UPROPERTY(Transient, BlueprintReadOnly)
	int32 CurrentStage = 1;

	UPROPERTY(Transient)
	int32 EnemyLevel = 1;

	UPROPERTY(Transient)
	TObjectPtr<AmsGameState> msGameState;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Artifact")
	TObjectPtr<UmsArtifactInfo> ArtifactInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Artifact")
	int32 Tier1_Weight = 50;
	UPROPERTY(EditDefaultsOnly, Category = "Artifact")
	int32 Tier2_Weight = 30;
	UPROPERTY(EditDefaultsOnly, Category = "Artifact")
	int32 Tier3_Weight = 20;

	UPROPERTY(EditDefaultsOnly, Category = "Artifact")
	int32 NumEvolution = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	FScalableFloat EenmyMinLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	FScalableFloat EnemyMaxLevel;
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	FScalableFloat EnemyDeathCount;

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	TArray<FString> LevelNames;

	UPROPERTY(Transient)
	TSet<AController*> DeadPlayers;

private:
	mutable TArray<FGameplayTag> CachedArtifactList_Tier1;
	mutable TArray<FGameplayTag> CachedArtifactList_Tier2;
	mutable TArray<FGameplayTag> CachedArtifactList_Tier3;

	mutable TMap<FGameplayTag, FArtifactInfo> CachedArtifactMap;

	int32 ObjectiveCount = 0;

	int32 NumEnemiesKilled = 0;

	FTimerHandle ProceedNextStageTimerHandle;
};
