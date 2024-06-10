// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "msGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FUpdateObjectiveCountSignature, int32 /* ObjectiveCount*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FUpdateStageNumSignature, int32 /* StageNum*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStageStageChangedSignature, FName /*StageState*/)

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API AmsGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void AddPlayerState(APlayerState* PlayerState) override;

	void UpdateObejctiveCount(const int32 InObjectiveCount);
	void UpdateStageNum(const int32 InStageNum);

	FUpdateObjectiveCountSignature UpdateObjectiveCountDelegate;
	FUpdateStageNumSignature UpdateStageNumDelegate;
	FOnStageStageChangedSignature StageStageChangedDelegate;

	int32 GetObjectiveCount() const { return ObjectiveCount; }
	int32 GetStageNum() const { return StageNum; }
	int32 GetEnemyLevel() const { return EnemyLevel; }

	void SetStageStage(FName NewState);

	void TeleportToStartPoint(AActor* StartPoint);

protected:
	void GiveEvolutionOption();

protected:
	UFUNCTION()
	void OnRep_ObjectiveCount();

	UFUNCTION()
	void OnRep_StageState();

	UFUNCTION()
	void OnRep_StageNum();

	void HandleStageState();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_ObjectiveCount)
	int32 ObjectiveCount;

	UPROPERTY(ReplicatedUsing = OnRep_StageState)
	FName StageState;

	UPROPERTY(ReplicatedUsing = OnRep_StageNum)
	int32 StageNum;

	UPROPERTY(Replicated)
	int32 EnemyLevel = 1;
};
