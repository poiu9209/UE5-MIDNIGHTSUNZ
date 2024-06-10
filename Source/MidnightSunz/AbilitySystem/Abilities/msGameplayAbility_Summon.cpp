// Fill out your copyright notice in the Description page of Project Settings.


#include "msGameplayAbility_Summon.h"
#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UmsGameplayAbility_Summon::GetSpawnLocations() const
{
	// 전방 각도만큼 범위
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector  Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float DeltaSpread = SpawnSpread / NumSummon;

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	TArray<FVector> SpawnLocations;
	for (int32 i = 0; i < NumSummon; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		// Distance 사이에 랜덤하게 스폰
		FVector ChosenSpawnLocation = Location + Direction * FMath::RandRange(MinSpawnDistance, MaxSpawnDistance);

		// 바닥 snap
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, ChosenSpawnLocation + FVector(0.f, 0.f, 400.f), ChosenSpawnLocation - FVector(0.f, 0.f, 400.f), ECollisionChannel::ECC_Visibility);
		if (Hit.bBlockingHit)
		{
			ChosenSpawnLocation = Hit.ImpactPoint;
		}
		SpawnLocations.Add(ChosenSpawnLocation);

		if (bDrawDebug)
		{
			DrawDebugSphere(GetWorld(), ChosenSpawnLocation, 18.f, 12, FColor::Cyan, false, 3.f);
			UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + Direction * MaxSpawnDistance, 4.f, FLinearColor::Green, 3.f);
			DrawDebugSphere(GetWorld(), Location + Direction * MinSpawnDistance, 5.f, 12, FColor::Red, false, 3.f);
			DrawDebugSphere(GetWorld(), Location + Direction * MaxSpawnDistance, 5.f, 12, FColor::Red, false, 3.f);
		}
	}

	return SpawnLocations;
}

TSubclassOf<AActor> UmsGameplayAbility_Summon::GetRandomSummonClass() const
{
	const int32 Selection = FMath::RandRange(0, SummonClasses.Num() - 1);
	return SummonClasses[Selection];
}
