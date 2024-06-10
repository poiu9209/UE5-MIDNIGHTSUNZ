// Fill out your copyright notice in the Description page of Project Settings.


#include "msGameState.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "MidnightSunz/GameModes/msGameTypes.h"
#include "MidnightSunz/GameModes/msGameMode.h"
#include "MidnightSunz/Player/msPlayerState.h"

void AmsGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AmsGameState, ObjectiveCount);
	DOREPLIFETIME(AmsGameState, StageState);
	DOREPLIFETIME(AmsGameState, StageNum);
	DOREPLIFETIME(AmsGameState, EnemyLevel);
}

void AmsGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	if (PlayerState && PlayerState->GetOwningController() && PlayerState->GetOwningController()->IsLocalPlayerController())
	{
		AmsPlayerState* msPlayerState = Cast<AmsPlayerState>(PlayerState);
		if (msPlayerState)
		{
			msPlayerState->InitGameState();
		}
	}
}

void AmsGameState::UpdateObejctiveCount(const int32 InObjectiveCount)
{
	ObjectiveCount = InObjectiveCount;

	UpdateObjectiveCountDelegate.Broadcast(ObjectiveCount);
}

void AmsGameState::UpdateStageNum(const int32 InStageNum)
{
	StageNum = InStageNum;

	UpdateStageNumDelegate.Broadcast(StageNum);
}

void AmsGameState::SetStageStage(FName NewState)
{
	if (StageState == NewState)
	{
		return;
	}

	StageState = NewState;
	HandleStageState();
}

void AmsGameState::TeleportToStartPoint(AActor* StartPoint)
{
	for (auto& Player : PlayerArray)
	{
		if (auto Pawn = Player->GetPawn())
		{
			Pawn->TeleportTo(StartPoint->GetActorLocation(), StartPoint->GetActorRotation());
		}
	}
}

void AmsGameState::GiveEvolutionOption()
{
	if (HasAuthority())
	{
		TArray<FGameplayTag> EvolutionTags;
		Cast<AmsGameMode>(UGameplayStatics::GetGameMode(this))->GetRandomEvolutionTag(EvolutionTags);

		FEvolutionOption Option;
		Option.EvolutionTags.Append(EvolutionTags);

		for (auto PS : PlayerArray)
		{
			AmsPlayerState* MyPS = CastChecked<AmsPlayerState>(PS);
			MyPS->AddEvolutionOpion(Option);
		}
	}
}

void AmsGameState::OnRep_ObjectiveCount()
{
	UpdateObjectiveCountDelegate.Broadcast(ObjectiveCount);
}

void AmsGameState::OnRep_StageState()
{
	HandleStageState();
}

void AmsGameState::HandleStageState()
{
	if (StageState == msStageState::StageStart)
	{
		// Init ObjectiveCount
		if (HasAuthority())
		{
			AmsGameMode* GM = Cast<AmsGameMode>(UGameplayStatics::GetGameMode(this));
			const int32 ObjCnt = GM->GetObjectiveCount();
			UpdateObejctiveCount(ObjCnt);

			const int32 Stage = GM->CurrentStage;
			UpdateStageNum(Stage);

			EnemyLevel = GM->EnemyLevel;
		}
	}
	else if (StageState == msStageState::WaitNextStage)
	{
		GiveEvolutionOption();
	}

	StageStageChangedDelegate.Broadcast(StageState);
}

void AmsGameState::OnRep_StageNum()
{
	UpdateStageNumDelegate.Broadcast(StageNum);
}
