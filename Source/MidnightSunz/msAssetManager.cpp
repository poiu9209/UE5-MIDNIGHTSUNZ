// Fill out your copyright notice in the Description page of Project Settings.


#include "msAssetManager.h"
#include "msGameplayTags.h"
#include "AbilitySystemGlobals.h"

UmsAssetManager& UmsAssetManager::Get()
{
	check(GEngine);

	UmsAssetManager* AssetManager = Cast<UmsAssetManager>(GEngine->AssetManager);
	return *AssetManager;
}

void UmsAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FmsGameplayTags::Get().InitializeNativeGameplayTags();

	UAbilitySystemGlobals::Get().InitGlobalData();
}
