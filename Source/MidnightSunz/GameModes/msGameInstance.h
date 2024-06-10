// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameInstance_Modified.h"
#include "MidnightSunz/GameModes/msGameTypes.h"
#include "msGameInstance.generated.h"

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API UmsGameInstance : public UGameInstance_Modified
{
	GENERATED_BODY()

public:
	void ClearData(const FString& Name);
	void SaveData(APlayerState* PS);
	bool GetSaveData(const FString& Name, FPlayerSaveData& OutData);

public:
	int32 CurrentStage = 1;

private:
	UPROPERTY()
	TMap<FString, FPlayerSaveData> PlayerSaveDataMap;

};
