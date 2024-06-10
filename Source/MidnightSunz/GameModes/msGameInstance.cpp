// Fill out your copyright notice in the Description page of Project Settings.


#include "msGameInstance.h"
#include "MidnightSunz/Player/msPlayerState.h"

void UmsGameInstance::ClearData(const FString& Name)
{
	if (PlayerSaveDataMap.Find(Name))
	{
		PlayerSaveDataMap[Name].Clear();
	}
}

void UmsGameInstance::SaveData(APlayerState* PS)
{
	FString PlayerName = PS->GetPlayerName();
	if (!PlayerSaveDataMap.Find(PlayerName))
	{
		PlayerSaveDataMap.Add(PlayerName);
	}

	FPlayerSaveData SaveData;
	Cast<AmsPlayerState>(PS)->GetSaveData(SaveData);
	PlayerSaveDataMap[PlayerName] = SaveData;
}

bool UmsGameInstance::GetSaveData(const FString& Name, FPlayerSaveData& OutData)
{
	if (!PlayerSaveDataMap.Find(Name))
	{
		return false;
	}

	OutData = PlayerSaveDataMap[Name];
	return true;
}
