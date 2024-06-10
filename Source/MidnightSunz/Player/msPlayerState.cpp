// Fill out your copyright notice in the Description page of Project Settings.


#include "msPlayerState.h"
#include "msPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "MidnightSunz/msGameplayTags.h"
#include "MidnightSunz/GameModes/msGameInstance.h"
#include "MidnightSunz/AbilitySystem/msAbilitySystemComponent.h"
#include "MidnightSunz/AbilitySystem/Attributes/msAttributeSet.h"
#include "MidnightSunz/AbilitySystem/Artifact/msArtifact.h"
#include "MidnightSunz/AbilitySystem/Abilities/msGameplayAbility_Damage.h"
#include "Kismet/GameplayStatics.h"
#include "MidnightSunz/GameModes/msGameMode.h"

AmsPlayerState::AmsPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UmsAbilitySystemComponent>(TEXT("AbilitySystemComponen"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UmsAttributeSet>(TEXT("AttributeSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	NetUpdateFrequency = 100.0f;
}

void AmsPlayerState::BeginPlay()
{
	Super::BeginPlay();

	RegisterArtifactEvent();
}

void AmsPlayerState::RegisterArtifactEvent()
{
	if (bRegisterArtifactEvent)
	{
		return;
	}

	const TArray<FGameplayTag>& ArtifactTags = FmsGameplayTags::Get().ArtifactTags;
	for (const auto& ArtifactTag : ArtifactTags)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(ArtifactTag, EGameplayTagEventType::AnyCountChange).AddUObject(this, &ThisClass::ArtifactChanged);
	}
	bRegisterArtifactEvent = true;
}

void AmsPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	const TArray<FGameplayTag>& ArtifactTags = FmsGameplayTags::Get().ArtifactTags;
	for (const auto& ArtifactTag : ArtifactTags)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(ArtifactTag, EGameplayTagEventType::AnyCountChange).RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

UAbilitySystemComponent* AmsPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UmsAttributeSet* AmsPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

void AmsPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AmsPlayerState, Level);
	DOREPLIFETIME_CONDITION(AmsPlayerState, EvolutionOptions, COND_OwnerOnly);
}

void AmsPlayerState::AddEvolutionOpion(const FEvolutionOption& InOption)
{
	EvolutionOptions.Add(InOption);
	OnUpdatePointDelegate.Broadcast(EvolutionOptions.Num());
}

void AmsPlayerState::ServerRequestEvolution_Implementation(const FGameplayTag& EvolutionTag)
{
	if (auto Spec = AbilitySystemComponent->LevelUpAbility(EvolutionTag))
	{
		if (Spec->DynamicAbilityTags.HasTagExact(FmsGameplayTags::Get().InputTag_LMB))
		{
			StoreBaseDamage();
		}
	}
	else
	{
		ArtifactUpgrade(EvolutionTag);
	}

	EvolutionOptions.RemoveAt(0);
	if (EvolutionOptions.Num() > 0)
	{
		ClientAckPointTagList(EvolutionOptions[0].EvolutionTags);
	}
}


void AmsPlayerState::ArtifactUpgrade(const FGameplayTag& InTag)
{
	if (ArtifactMap.Find(InTag))
	{
		AddArtifactStack(InTag);
	}
	else
	{
		NewArtifact(InTag, 1);
	}
}

void AmsPlayerState::AddArtifactStack(const FGameplayTag& InTag)
{
	AmsGameMode* GM = CastChecked<AmsGameMode>(UGameplayStatics::GetGameMode(this));
	auto ArtifactClass = GM->EvolutionArtifact(InTag, GetAbilitySystemComponent());

	ArtifactMap[InTag]->AddStack();
}

void AmsPlayerState::NewArtifact(const FGameplayTag& InTag, int32 InStack)
{
	AmsGameMode* GM = CastChecked<AmsGameMode>(UGameplayStatics::GetGameMode(this));
	auto ArtifactClass = GM->EvolutionArtifact(InTag, GetAbilitySystemComponent(), InStack);

	UmsArtifact* NewArtifact = NewObject<UmsArtifact>(this, ArtifactClass);
	NewArtifact->InitArtifact(InTag, InStack);
	ArtifactMap.Add(InTag, NewArtifact);

	if (UmsArtifact_KillEnemy* KillEnemyArtifact = Cast<UmsArtifact_KillEnemy>(NewArtifact))
	{
		KillEnemyArtifacts.Emplace(KillEnemyArtifact);
	}
	else if (UmsArtifact_HitEnemy* HitEnemyArtifact = Cast<UmsArtifact_HitEnemy>(NewArtifact))
	{
		HitEnemyArtifacts.Emplace(HitEnemyArtifact);
	}
	else if (UmsArtifact_NormalAttack* NormalAttackArtifact = Cast<UmsArtifact_NormalAttack>(NewArtifact))
	{
		NormalAttackArtifacts.Emplace(NormalAttackArtifact);
	}
}

void AmsPlayerState::RequestEvolutionTags()
{
	if (EvolutionOptions.Num() <= 0)
	{
		return;
	}

	OnUpdatePointTagListDelegate.Broadcast(EvolutionOptions[0].EvolutionTags);
}

void AmsPlayerState::ArtifactChanged(const FGameplayTag ArtifactTag, int32 NewCount)
{
	UE_LOG(LogTemp, Log, TEXT("[%s] ArtifactChanged [%d] , [%s]"),
		*ArtifactTag.GetTagName().ToString(),
		NewCount,
		*GetNameSafe(this));

	if (ArtifactInfoMap.Find(ArtifactTag))
	{
		ArtifactInfoMap[ArtifactTag] = NewCount;
	}
	else
	{
		ArtifactInfoMap.Add(ArtifactTag, NewCount);
	}
	OnUpdateArtifactDelegate.Broadcast(ArtifactTag, NewCount);
}

void AmsPlayerState::ClientAckPointTagList_Implementation(const TArray<FGameplayTag>& TagList)
{
	OnUpdatePointTagListDelegate.Broadcast(TagList);
}

void AmsPlayerState::OnRep_Level(int32 OldLevel)
{
}

void AmsPlayerState::OnRep_EvolutionOptions()
{
	OnUpdatePointDelegate.Broadcast(EvolutionOptions.Num());
}

int32 AmsPlayerState::GetArtifactStack(const FGameplayTag& InTag) const
{
	if (ArtifactInfoMap.Find(InTag))
	{
		return ArtifactInfoMap[InTag];
	}
	return 0;
}

void AmsPlayerState::TriggerKillEnemyArtifacts(const FVector& TargetLocation)
{
	for (const auto& Artifact : KillEnemyArtifacts)
	{
		if (Artifact)
		{
			Artifact->Trigger(GetPawn(), TargetLocation);
		}
	}
}

void AmsPlayerState::TriggerHitEnemyArtifacts(AActor* HitActor)
{
	for (const auto& Artifact : HitEnemyArtifacts)
	{
		if (Artifact)
		{
			Artifact->Trigger(GetPawn(), HitActor);
		}
	}
}

void AmsPlayerState::TriggerNormalAttackArtifact()
{
	for (const auto& Artifact : NormalAttackArtifacts)
	{
		if (Artifact)
		{
			Artifact->Trigger(GetPawn());
		}
	}
}

void AmsPlayerState::StoreBaseDamage()
{
	if (auto AttackSpec = AbilitySystemComponent->GetSpecFromInputTag(FmsGameplayTags::Get().InputTag_LMB))
	{
		if (auto Attack = Cast<UmsGameplayAbility_Damage>(AttackSpec->Ability))
		{
			BaseDamage = Attack->GetDamage(AttackSpec->Level);
		}
	}
}

void AmsPlayerState::CheatAddSkillLevel()
{
	AbilitySystemComponent->CheatAddSkillLevel();
}

float AmsPlayerState::GetArtifactProbability(const FGameplayTag& InArtifactTag) const
{
	if (const auto FindArtifact = ArtifactMap.Find(InArtifactTag))
	{
		return FindArtifact->Get()->GetProbability();
	}

	return 0.0f;
}

bool AmsPlayerState::CheckArtifactProbability(const FGameplayTag& InArtifactTag) const
{
	if (const auto FindArtifact = ArtifactMap.Find(InArtifactTag))
	{
		return FindArtifact->Get()->CheckProbability();
	}

	return false;
}

void AmsPlayerState::GetSaveData(FPlayerSaveData& OutData)
{
	MulticastDiableGameplay();

	AbilitySystemComponent->GetSkillAbilityLevel(OutData);
	OutData.ArtifactLevel.Empty();
	for (auto& Art : ArtifactMap)
	{
		OutData.ArtifactLevel.Add(Art.Key, GetArtifactStack(Art.Key));
	}
	OutData.Evolutions = EvolutionOptions;
}

void AmsPlayerState::MulticastDiableGameplay_Implementation()
{
	if (AmsPlayerController* MyPC = Cast<AmsPlayerController>(GetPlayerController()))
	{
		MyPC->SetDiableGameplay(true);
	}
}

void AmsPlayerState::ApplySavedArtifactAndEvolution(FPlayerSaveData& Data)
{
	RegisterArtifactEvent();
	for (auto& Artifact : Data.ArtifactLevel)
	{
		NewArtifact(Artifact.Key, Artifact.Value);
	}
	EvolutionOptions = Data.Evolutions;
	OnUpdatePointDelegate.Broadcast(EvolutionOptions.Num());
}

void AmsPlayerState::InitGameState()
{
	if (OnInitGameState.IsBound())
	{
		OnInitGameState.Broadcast();
	}
}
