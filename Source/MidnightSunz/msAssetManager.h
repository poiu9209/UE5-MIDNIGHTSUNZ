// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "msAssetManager.generated.h"

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API UmsAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UmsAssetManager& Get();

protected:
	virtual void StartInitialLoading() override;
};
