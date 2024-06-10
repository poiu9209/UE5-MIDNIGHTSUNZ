// Fill out your copyright notice in the Description page of Project Settings.


#include "msBTService_FindNearestPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "MidnightSunz/Interface/msCombatInterface.h"
#include "BehaviorTree/BTFunctionLibrary.h"

void UmsBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* OwningPawn = AIOwner->GetPawn();
	if (!IsValid(OwningPawn))
	{
		return;
	}

	// AI가 꼭 펫일리는없음 나중에 펫같은게 생기면 Enemy를 찾아야하므로  tag분기를둠.
	const FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithTag(this, TargetTag, Actors);

	float ClosestDistance = TNumericLimits<float>::Max();
	AActor* ClosestActor = nullptr;
	for (auto Actor : Actors)
	{
		if (IsValid(Actor) && IsValid(OwningPawn))
		{
			if (Actor->Implements<UmsCombatInterface>())
			{
				if (ImsCombatInterface::Execute_IsDead(Actor))
				{
					continue;
				}
			}

			const float Distance = OwningPawn->GetDistanceTo(Actor);
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestActor = Actor;
			}
		}
	}

	UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, ClosestActor);
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector, ClosestDistance);
}
