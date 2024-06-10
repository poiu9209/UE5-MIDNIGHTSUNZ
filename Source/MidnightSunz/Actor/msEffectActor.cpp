// Fill out your copyright notice in the Description page of Project Settings.


#include "msEffectActor.h"
#include "MidnightSunz/Interface/msCombatInterface.h"
#include "MidnightSunz/AbilitySystem/msAbilitySystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

AmsEffectActor::AmsEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AmsEffectActor::Destroyed()
{
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}
	if (PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, GetActorLocation(), GetActorRotation());
	}

	Super::Destroyed();
}

bool AmsEffectActor::ShouldApplyEffectToTarget(AActor* TargetActor) const
{
	if (!GameplayEffectClass || !IsValid(TargetActor) || (!bApplyEffectsToEnemies && TargetActor->ActorHasTag("Enemy")))
	{
		return false;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC)
	{
		return false;
	}

	ImsCombatInterface* CombatInterface = Cast<ImsCombatInterface>(TargetActor);
	if (!CombatInterface)
	{
		return false;
	}

	// ���� ����� Stack������ GE�� Stack ���� ���ؼ� ����ȵǴ°Ÿ� ����.
	UGameplayEffect* EffectCDO = GameplayEffectClass.GetDefaultObject();
	const int32 StackLimit = EffectCDO->StackLimitCount;
	if (StackLimit > 0 && EffectCDO->InheritableOwnedTagsContainer.CombinedTags.Num() > 0)
	{
		// ���� �±׿� ������ Ȱ�� ȿ�� �ڵ� �迭 ��������
		TArray<FActiveGameplayEffectHandle> BuffActiveEffectHandles;
		for (const FGameplayTag& InheritableTag : EffectCDO->InheritableOwnedTagsContainer.CombinedTags)
		{
			TArray<FActiveGameplayEffectHandle> TargetActorActiveEffectHandles;
			CombatInterface->GetBuffActiveEffectHandles(InheritableTag, TargetActorActiveEffectHandles);

			BuffActiveEffectHandles.Append(TargetActorActiveEffectHandles);
		}

		// ���� �±׺��� ����� ���� �� Ȯ��
		for (const FActiveGameplayEffectHandle& ActiveEffectHandle : BuffActiveEffectHandles)
		{
			const int32 CurStack = TargetASC->GetCurrentStackCount(ActiveEffectHandle);
			if (StackLimit > 0 && CurStack >= StackLimit)
			{
				return false;
			}
		}
	}

	return true;
}

void AmsEffectActor::ApplyEffectToTarget(AActor* TargetActor, bool bDestroy /*= true*/)
{
	if (!ShouldApplyEffectToTarget(TargetActor))
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle	EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data);

	if (bDestroy)
	{
		Destroy();
	}
}