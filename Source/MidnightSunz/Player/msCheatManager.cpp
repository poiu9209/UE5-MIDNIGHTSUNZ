// Fill out your copyright notice in the Description page of Project Settings.


#include "msCheatManager.h"
#include "Kismet/GameplayStatics.h"
#include "Gameframework/Character.h"
#include "MidnightSunz/Player/msPlayerState.h"
#include "MidnightSunz/GameModes/msGameMode.h"
#include "MidnightSunz/AbilitySystem/Attributes/msAttributeSet.h"
#include "MidnightSunz/Interface/msCombatInterface.h"
#include "MidnightSunz/msGameplayTags.h"

void UmsCheatManager::AddArtifact(const FGameplayTag& InTag)
{
	GetOuterAPlayerController()->GetPlayerState<AmsPlayerState>()->ArtifactUpgrade(InTag);
}

void UmsCheatManager::AddAllArtifact()
{
	for (const auto& ArtifactTag : FmsGameplayTags::Get().ArtifactTags)
	{
		GetOuterAPlayerController()->GetPlayerState<AmsPlayerState>()->ArtifactUpgrade(ArtifactTag);
	}
}

void UmsCheatManager::AddSkillLevel()
{
#if WITH_EDITOR
	GetOuterAPlayerController()->GetPlayerState<AmsPlayerState>()->CheatAddSkillLevel();
	GetOuterAPlayerController()->GetPlayerState<AmsPlayerState>()->StoreBaseDamage();
#endif
}

void UmsCheatManager::ProceedObjectiveComplete()
{
	if (AmsGameMode* GM = Cast<AmsGameMode>(UGameplayStatics::GetGameMode(GetOuterAPlayerController())))
	{
		GM->ObjectiveComplete();
	}
}

void UmsCheatManager::Suicide()
{
	GetOuterAPlayerController()->GetPlayerState<AmsPlayerState>()->GetAttributeSet()->SetHealth(0.f);
	Cast<ImsCombatInterface>(GetOuterAPlayerController()->GetPawn())->Die();
}

void UmsCheatManager::Revive()
{
	if (AmsGameMode* GM = Cast<AmsGameMode>(UGameplayStatics::GetGameMode(GetOuterAPlayerController())))
	{
		GM->RequestRespawn(Cast<ACharacter>(GetOuterAPlayerController()->GetPawn()), GetOuterAPlayerController());
	}
}
