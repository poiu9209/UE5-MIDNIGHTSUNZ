// Fill out your copyright notice in the Description page of Project Settings.


#include "msGameMode.h"
#include "MidnightSunz/MidnightSunz.h"
#include "MidnightSunz/msGameplayTags.h"
#include "MidnightSunz/Player/msPlayerState.h"
#include "MidnightSunz/Player/msPlayerController.h"
#include "MidnightSunz/Character/msEnemy.h"
#include "MidnightSunz/Character/msCharacter.h"
#include "MidnightSunz/AbilitySystem/Data/msAbilityInfo.h"
#include "msGameState.h"
#include "msGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "AbilitySystemComponent.h"
#include "EngineUtils.h"

namespace msStageState
{
	const FName None = FName("None");
	const FName StageStart = FName("StageStart");
	const FName WaitNextStage = FName("WaitNextStage");
	const FName GameOver = FName("GameOver");
}

void AmsGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (GetGameInstance<UmsGameInstance>())
	{
		CurrentStage = GetGameInstance<UmsGameInstance>()->CurrentStage;
		const int32 Min = EenmyMinLevel.GetValueAtLevel(CurrentStage);
		const int32 Max = EnemyMaxLevel.GetValueAtLevel(CurrentStage);
		EnemyLevel = FMath::RandRange(Min, Max);
	}
	CacheArtifactList();
}

void AmsGameMode::CacheArtifactList() const
{
	CachedArtifactMap.Empty();
	CachedArtifactList_Tier1.Empty();
	CachedArtifactList_Tier2.Empty();
	CachedArtifactList_Tier3.Empty();

	for (const auto& Tier_1_Info : ArtifactInfo->ArtifactData.Tier1Artifacts)
	{
		CachedArtifactMap.Add(Tier_1_Info.Tag, Tier_1_Info);
		CachedArtifactList_Tier1.AddUnique(Tier_1_Info.Tag);
	}
	for (const auto& Tier_2_Info : ArtifactInfo->ArtifactData.Tier2Artifacts)
	{
		CachedArtifactMap.Add(Tier_2_Info.Tag, Tier_2_Info);
		CachedArtifactList_Tier2.AddUnique(Tier_2_Info.Tag);
	}
	for (const auto& Tier_3_Info : ArtifactInfo->ArtifactData.Tier3Artifacts)
	{
		CachedArtifactMap.Add(Tier_3_Info.Tag, Tier_3_Info);
		CachedArtifactList_Tier3.AddUnique(Tier_3_Info.Tag);
	}
}

void AmsGameMode::InitGameState()
{
	Super::InitGameState();

	msGameState = GetGameState<AmsGameState>();
	check(msGameState);
}

void AmsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	const bool IsInProgress = IsMatchInProgress();
	if (IsMatchInProgress())
	{
		UE_LOG(LogTemp, Warning, TEXT("Late Join %s"), *GetNameSafe(NewPlayer));
		if (AmsPlayerController* PC = Cast<AmsPlayerController>(NewPlayer))
		{
			PC->OnStageStateSet(StageState);
		}
	}
}

AActor* AmsGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
	for (auto Start : PlayerStarts)
	{
		APlayerStart* PlayerStart = Cast<APlayerStart>(Start);
		if (PlayerStart && PlayerStart->PlayerStartTag == PLAYER_START_TAG)
		{
			return Start;
		}
	}

	return Super::FindPlayerStart_Implementation(Player, IncomingName);
}

void AmsGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	ObjectiveCount = GetObjectiveCount();
	msGameState->UpdateObejctiveCount(ObjectiveCount);

	SetStageState(msStageState::StageStart);
}

bool AmsGameMode::AllowCheats(APlayerController* P)
{
	return true;
}

void AmsGameMode::HandleCharacterDeath(ACharacter* DeadCharacter)
{
	if (!StageState.IsEqual(msStageState::StageStart))
	{
		return;
	}

	if (!DeadCharacter)
	{
		return;
	}

	AmsEnemy* Enemy = Cast<AmsEnemy>(DeadCharacter);
	if (Enemy)
	{
		NumEnemiesKilled++;

		msGameState->UpdateObejctiveCount(FMath::Max(ObjectiveCount - NumEnemiesKilled, 0));

		if (NumEnemiesKilled >= ObjectiveCount)
		{
			ObjectiveComplete();
		}
	}
	else if (AmsCharacter* Player = Cast<AmsCharacter>(DeadCharacter))
	{
		DeadPlayers.Add(Player->GetController());

		if (DeadPlayers.Num() >= 2)
		{
			SetStageState(msStageState::GameOver);
		}
	}
}

int32 AmsGameMode::GetObjectiveCount() const
{
	return EnemyDeathCount.GetValueAtLevel(CurrentStage);
}

TSubclassOf<UmsArtifact> AmsGameMode::EvolutionArtifact(const FGameplayTag& InTag, UAbilitySystemComponent* ASC, uint8 InStack /*= 1*/) const
{
	const FArtifactInfo* FindArtifact = CachedArtifactMap.Find(InTag);
	if (!FindArtifact)
	{
		CacheArtifactList();
		FindArtifact = CachedArtifactMap.Find(InTag);
	}
	check(FindArtifact);

	TSubclassOf<UGameplayEffect> Effect = FindArtifact->ArtifactEffect;
	if (!Effect)
	{
		return nullptr;
	}

	// Apply Effect
	for (uint8 i = 0; i < InStack; i++)
	{
		FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
		const FGameplayEffectSpecHandle	EffectSpecHandle = ASC->MakeOutgoingSpec(Effect, 1.f, EffectContextHandle);
		ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);
	}

	return FindArtifact->ArtifactObj;
}

void AmsGameMode::SetStageState(FName NewState)
{
	if (StageState == NewState)
	{
		return;
	}

	StageState = NewState;
	StageChangeDelegate.Broadcast(NewState);
	OnStageStateSet();
}

void AmsGameMode::OnStageStateSet()
{
	ObjectiveCount = GetObjectiveCount();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AmsPlayerController* PC = Cast<AmsPlayerController>(*It))
		{
			PC->OnStageStateSet(StageState);
		}
	}

	if (StageState == msStageState::WaitNextStage || StageState == msStageState::GameOver)
	{
		TArray<AActor*> SummonActors;
		UGameplayStatics::GetAllActorsWithTag(this, SUMMON_TAG, SummonActors);
		for (auto Summon : SummonActors)
		{
			if (IsValid(Summon) && Summon->Implements<UmsCombatInterface>() && !ImsCombatInterface::Execute_IsDead(Summon))
			{
				Cast<ImsCombatInterface>(Summon)->Die();
			}
		}
	}

	msGameState->SetStageStage(StageState);
}

TArray<FGameplayTag>& AmsGameMode::GetArtifactListByRandomTier()
{
	const int32 RandTier = FMath::RandRange(1, Tier1_Weight + Tier2_Weight + Tier3_Weight);
	if (RandTier <= Tier1_Weight)
	{
		return CachedArtifactList_Tier1;
	}
	else if (RandTier <= Tier1_Weight + Tier2_Weight)
	{
		return CachedArtifactList_Tier2;
	}
	else
	{
		return CachedArtifactList_Tier3;
	}
}

void AmsGameMode::GetRandomEvolutionTag(TArray<FGameplayTag>& OutTag)
{
	const EPointType Point = CurrentStage % 2 == 0 ? EPointType::Artifact : EPointType::AbilityLevel;
	if (Point == EPointType::AbilityLevel)
	{
		TArray<FGameplayTag> SkillTags = FmsGameplayTags::Get().SkillAbilityTags;
		for (int32 i = 0; i < NumEvolution; i++)
		{
			const int32 RandIndx = FMath::RandRange(0, SkillTags.Num() - 1);
			OutTag.Add(SkillTags[RandIndx]);
			SkillTags.RemoveAtSwap(RandIndx);
		}
	}
	else
	{
		TArray<FGameplayTag>& ArtifactList = GetArtifactListByRandomTier();
		for (int32 i = 0; i < NumEvolution; i++)
		{
			if (ArtifactList.Num() <= 0)
			{
				return;
			}

			const int32 RandIndx = FMath::RandRange(0, ArtifactList.Num() - 1);
			OutTag.Add(ArtifactList[RandIndx]);
			ArtifactList.RemoveAtSwap(RandIndx);
		}
	}
}

void AmsGameMode::NextStageStart()
{
	if (StageState != msStageState::WaitNextStage)
	{
		return;
	}

	UmsGameInstance* GI = GetGameInstance<UmsGameInstance>();
	for (auto& Player : GameState->PlayerArray)
	{
		GI->SaveData(Player);
	}

	FString NextLevel = LevelNames[(CurrentStage - 1) % LevelNames.Num()];
	FString URL = FString::Printf(TEXT("/Game/Maps/%s?listen"), *NextLevel);
	GetWorld()->ServerTravel(URL);
}

void AmsGameMode::ObjectiveComplete()
{
	ObjectiveCompleteDelegate.Broadcast();

	auto g = GetGameInstance();
	UmsGameInstance* GI = GetGameInstance<UmsGameInstance>();

	NumEnemiesKilled = 0;
	CurrentStage++;
	GI->CurrentStage = CurrentStage;

	SetStageState(msStageState::WaitNextStage);
	GetWorldTimerManager().SetTimer(ProceedNextStageTimerHandle, this, &ThisClass::NextStageStart, 5.f, false);
}

void AmsGameMode::RequestRespawn(ACharacter* InCharacter, AController* InController)
{
	if (StageState == msStageState::GameOver)
	{
		return;
	}

	FVector DeathLocation = FVector::ZeroVector;
	if (InCharacter)
	{
		DeathLocation = InCharacter->GetActorLocation();
		InCharacter->Reset();
		InCharacter->Destroy();
	}

	if (InController)
	{
		DeadPlayers.Remove(InController);
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		float ClosestDistance = TNumericLimits<float>::Max();
		AActor* ClosestPlayerStart = nullptr;

		for (AActor* Start : PlayerStarts)
		{
			const float Distance = (DeathLocation - Start->GetActorLocation()).Size();
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestPlayerStart = Start;
			}
		}
		RestartPlayerAtPlayerStart(InController, ClosestPlayerStart);
	}
}
