// Fill out your copyright notice in the Description page of Project Settings.


#include "msAbilityTask_TargetUnderMouse.h"
#include "AbilitySystemComponent.h"

UmsAbilityTask_TargetUnderMouse* UmsAbilityTask_TargetUnderMouse::CreateTargetUnderMouse(UGameplayAbility* OwningAbility)
{
	UmsAbilityTask_TargetUnderMouse* Obj = NewAbilityTask<UmsAbilityTask_TargetUnderMouse>(OwningAbility);
	return Obj;
}

void UmsAbilityTask_TargetUnderMouse::Activate()
{
	Super::Activate();

	CachedPawn = Cast<APawn>(Ability->GetCurrentActorInfo()->AvatarActor);
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		SendMouseCursorData();
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &ThisClass::OnTargetDataReplicatedCallback);
		// ���ε��Ǳ����� ��������Ʈ�� �̹� ��ε�ĳ��Ʈ �Ǿ��ٸ� �ٽ� ��ε�ĳ��Ʈ �ϰ� ����.
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if (!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UmsAbilityTask_TargetUnderMouse::SendMouseCursorData()
{
	FScopedPredictionWindow ScopePrediction(AbilitySystemComponent.Get());

	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);

	const FVector HitLocation = CursorHit.Location;
	// Pawn �� �ƴѰ�� Z�� ����
	float CursorLocation_Z = HitLocation.Z;
	AActor* TargetActor = CursorHit.GetActor();
	if (!TargetActor)
	{
		CursorLocation_Z = CachedPawn->BaseEyeHeight;
	}
	else
	{
		if (TargetActor->IsA<APawn>())
		{
			CursorLocation_Z = CursorLocation_Z;
		}
		else
		{
			CursorLocation_Z = CachedPawn->BaseEyeHeight;

		}
	}

	CursorHit.Location = FVector(HitLocation.X, HitLocation.Y, CursorLocation_Z);


	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	AbilitySystemComponent->ServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(), DataHandle, FGameplayTag(), AbilitySystemComponent->ScopedPredictionKey);
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UmsAbilityTask_TargetUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	// Ÿ�ٵ����� ����, ĳ���Ѱ� �������ȴٶ�� �˷������
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
