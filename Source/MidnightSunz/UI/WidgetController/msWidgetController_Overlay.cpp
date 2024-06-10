// Fill out your copyright notice in the Description page of Project Settings.


#include "msWidgetController_Overlay.h"
#include "Kismet/GameplayStatics.h"
#include "MidnightSunz/AbilitySystem/Attributes/msAttributeSet.h"
#include "MidnightSunz/AbilitySystem/msAbilitySystemComponent.h"
#include "MidnightSunz/AbilitySystem/Data/msAbilityInfo.h"
#include "MidnightSunz/Interface/msCombatInterface.h"
#include "MidnightSunz/GameModes/msGameState.h"
#include "MidnightSunz/GameModes/msGameMode.h"
#include "MidnightSunz/Player/msPlayerController.h"
#include "MidnightSunz/Player/msPlayerState.h"

void UmsWidgetController_Overlay::BroadcastInitialValues()
{
	OnHealthChange.Broadcast(AttributeSet->GetHealth());
	OnMaxHealthChange.Broadcast(AttributeSet->GetMaxHealth());

	OnInitializeStartupAbilities(AbilitySystemComponent);

	BindGameStateDelegate();

	ChangePointDelegate.Broadcast(PlayerState->GetEvolutionPointNum());
}

void UmsWidgetController_Overlay::BindCallbacksToDependencies()
{
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) { OnHealthChange.Broadcast(Data.NewValue);	});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data) { OnMaxHealthChange.Broadcast(Data.NewValue); });
	AbilitySystemComponent->AbilityStatusChangedDelegate.AddLambda([this](const FGameplayTag& AbilityTag, int32 NewLevel)
		{
			OnChangedAbilityStatusDelegate.Broadcast(AbilityTag, NewLevel);
		});

	// 이미 어빌리티 부여되있다면 바인딩안하고 호출만
	if (AbilitySystemComponent->IsStartupAbilitiesGiven())
	{
		OnInitializeStartupAbilities(AbilitySystemComponent);
	}
	else
	{
		AbilitySystemComponent->AbilitiesGivenDelegate.AddUObject(this, &ThisClass::OnInitializeStartupAbilities);
	}

	if (ImsCombatInterface* CombatInterface = Cast<ImsCombatInterface>(AbilitySystemComponent->GetAvatarActor()))
	{
		CombatInterface->GetOnBuffChangeDelegate().AddDynamic(this, &ThisClass::BuffTagChanged);
	}

	if (AmsGameState* GS = Cast<AmsGameState>(UGameplayStatics::GetGameState(PlayerController)))
	{
		BindGameStateDelegate();
	}
	else
	{
		PlayerState->OnInitGameState.AddUObject(this, &ThisClass::BindGameStateDelegate);
	}

	PlayerState->OnUpdatePointDelegate.AddLambda([this](int32 Point)
		{
			ChangePointDelegate.Broadcast(Point);
		});

	PlayerState->OnUpdatePointTagListDelegate.AddLambda([this](const TArray<FGameplayTag>& Tags)
		{
			ChangePointTagListDelegate.Broadcast(Tags);
		});

	PlayerState->OnUpdateArtifactDelegate.AddLambda([this](const FGameplayTag& Tag, int32 Stack)
		{
			ChangeArtifactDelegate.Broadcast(Tag, Stack);
		});
}

void UmsWidgetController_Overlay::PointButtonPressed()
{
	PlayerState->RequestEvolutionTags();
}

void UmsWidgetController_Overlay::EvolutionSlotPressed(const FGameplayTag& EvolutionTag)
{
	PlayerState->ServerRequestEvolution(EvolutionTag);
}

FGameplayTag UmsWidgetController_Overlay::GetAbilityTagFromInputTag(const FGameplayTag& InputTag)
{
	if (FGameplayAbilitySpec* Spec = AbilitySystemComponent->GetSpecFromInputTag(InputTag))
	{
		return AbilitySystemComponent->GetAbilityTagFromSpec(*Spec);
	}
	return FGameplayTag::EmptyTag;
}

int32 UmsWidgetController_Overlay::GetArtifactLevel(const FGameplayTag& InTag)
{
	return PlayerState->GetArtifactStack(InTag);
}

int32 UmsWidgetController_Overlay::GetAbilityLevel(const FGameplayTag& InTag)
{
	if (auto Spec = AbilitySystemComponent->GetSpecFromAbilityTag(InTag))
	{
		return Spec->Level;
	}

	return 0;
}

void UmsWidgetController_Overlay::GetEvolutionText(const FGameplayTag& InTag, int32 InLevel, FText& OutTitle, FText& OutDesc)
{
	const bool bSkill = InTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities")));
	if (bSkill)
	{
		if (auto Spec = AbilitySystemComponent->GetSpecFromAbilityTag(InTag))
		{
			OutTitle = FText::FromStringTable("/Game/Blueprints/UI/Data/ST_Skill.ST_Skill", FString::Printf(TEXT("title_%s"), *InTag.ToString()));
			AbilitySystemComponent->GetAbilityEvolutionText(Spec, Spec->Level, OutDesc);
		}
	}
	else
	{
		OutTitle = FText::FromStringTable("/Game/Blueprints/UI/Data/ST_Artifact.ST_Artifact", FString::Printf(TEXT("title_%s"), *InTag.ToString()));
		OutDesc = FText::FromStringTable("/Game/Blueprints/UI/Data/ST_Artifact.ST_Artifact", FString::Printf(TEXT("evolution_%s"), *InTag.ToString()));
	}
}

void UmsWidgetController_Overlay::GetAbilityText(const FGameplayTag& InTag, int32 InLevel, FText& OutTitle, FText& OutDesc)
{
	if (auto Spec = AbilitySystemComponent->GetSpecFromAbilityTag(InTag))
	{
		OutTitle = FText::FromStringTable("/Game/Blueprints/UI/Data/ST_Skill.ST_Skill", FString::Printf(TEXT("title_%s"), *InTag.ToString()));
		AbilitySystemComponent->GetAbilityDescText(Spec, InLevel, OutDesc);
	}
}

void UmsWidgetController_Overlay::OnInitializeStartupAbilities(UmsAbilitySystemComponent* ASC)
{
	if (!ASC->IsStartupAbilitiesGiven())
	{
		return;
	}

	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this, ASC](const FGameplayAbilitySpec& AbilitySpec)
		{
			FAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(ASC->GetAbilityTagFromSpec(AbilitySpec));
			Info.InputTag = ASC->GetInputTagFromSpec(AbilitySpec);
			AbilityInfoDelegate.Broadcast(Info);
		});

	ASC->ForEachAbility(BroadcastDelegate);
}

void UmsWidgetController_Overlay::BindGameStateDelegate()
{
	if (AmsGameState* GS = Cast<AmsGameState>(UGameplayStatics::GetGameState(PlayerController)))
	{
		GS->UpdateObjectiveCountDelegate.AddLambda([this](int32 ObjectiveCount)
			{
				OnUpdateObjectiveCountDelegate.Broadcast(ObjectiveCount);
			});

		GS->UpdateStageNumDelegate.AddLambda([this](int32 StageNum)
			{
				OnUpdateStageNumDelegate.Broadcast(StageNum);
			});

		GS->StageStageChangedDelegate.AddLambda([this](FName StageState)
			{
				if (StageState == "StageStart")
				{
					StartStartDelegate.Broadcast();
				}
				else if (StageState == "WaitNextStage")
				{
					WaitStageDelegate.Broadcast();
				}
				else if (StageState == "GameOver")
				{
					GameOverDelegate.Broadcast();
				}
			});

		OnUpdateObjectiveCountDelegate.Broadcast(GS->GetObjectiveCount());
		OnUpdateStageNumDelegate.Broadcast(GS->GetStageNum());
	}
}

void UmsWidgetController_Overlay::BuffTagChanged(const FGameplayTag& InBuffTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		OnNewBuffDelegate.Broadcast(InBuffTag);
	}
}