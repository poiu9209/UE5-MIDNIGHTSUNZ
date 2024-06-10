// Fill out your copyright notice in the Description page of Project Settings.


#include "msGameplayAbility_TeslaChain.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "MidnightSunz/msGameplayTags.h"
#include "MidnightSunz/Interface/msCombatInterface.h"
#include "MidnightSunz/AbilitySystem/msAbilitySystemLibrary.h"

void UmsGameplayAbility_TeslaChain::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	SourceActor = Cast<AActor>(GetSourceObject(Handle, ActorInfo));

	// Radius 거리내의 Enemy 를 찾는다.
	StoreElectrocuteTargets();

	ApplyElectrocuteEffect();

	AddElectrocuteCue(GetAvatarActorFromActorInfo());

	for (auto Target : DamageToTargets)
	{
		if (Target.IsValid())
		{
			AddElectrocuteCue(Target.Get());
		}
	}

	if (HasAuthority(&ActivationInfo))
	{
		GetAvatarActorFromActorInfo()->GetWorld()->GetTimerManager().SetTimer(EndTimerHandle, this, &ThisClass::ExpiredElectocute, ElectocuteDuration, false);
		GetAvatarActorFromActorInfo()->GetWorld()->GetTimerManager().SetTimer(DamageTimerHandle, this, &ThisClass::ApplyEletrocuteDamage, DamageFrequency, true);
	}
}

void UmsGameplayAbility_TeslaChain::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	GetAvatarActorFromActorInfo()->GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	ClearAllElectrocuteEffects();

	RemoveElectrocuteCue(GetAvatarActorFromActorInfo());
	for (auto Target : DamageToTargets)
	{
		if (Target.IsValid())
		{
			RemoveElectrocuteCue(Target.Get());
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UmsGameplayAbility_TeslaChain::StoreElectrocuteTargets()
{
	AActor* AvataActor = GetAvatarActorFromActorInfo();
	check(AvataActor);
	DamageToTargets.Empty();

	ImsCombatInterface* CombatInterface = CastChecked<ImsCombatInterface>(AvataActor);
	CombatInterface->GetOnDeathDelegate().AddDynamic(this, &ThisClass::AvataActorDied);

	TArray<AActor*> OverlappingActors;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(AvataActor);

	UmsAbilitySystemLibrary::GetLivePlayersWithinRadius(AvataActor, OverlappingActors, IgnoreActors, ElectocuteRadius, AvataActor->GetActorLocation());

	// 가까운순으로 
	const FVector AvataLocation = AvataActor->GetActorLocation();
	OverlappingActors.Sort([&AvataLocation](const AActor& lhs, const AActor& rhs)
		{
			float DistLhsToAvata = FVector::Distance(lhs.GetActorLocation(), AvataLocation);
			float DistRhsToAvata = FVector::Distance(rhs.GetActorLocation(), AvataLocation);
			return DistLhsToAvata < DistRhsToAvata;
		});

	for (auto OverlapActor : OverlappingActors)
	{
		if (DamageToTargets.Num() > NumElectrocute)
		{
			break;
		}

		// 이미 감전중이면 제외
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OverlapActor);
		if (!TargetASC || TargetASC->HasMatchingGameplayTag(FmsGameplayTags::Get().Debuff_Lightning))
		{
			continue;
		}

		if (UmsAbilitySystemLibrary::IsNotFriend(AvataActor, OverlapActor))
		{
			continue;
		}

		ImsCombatInterface* TargetCombatInterface = CastChecked<ImsCombatInterface>(OverlapActor);
		if (!TargetCombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &ThisClass::ElectrocuteTargetDied))
		{
			TargetCombatInterface->GetOnDeathDelegate().AddDynamic(this, &ThisClass::ElectrocuteTargetDied);
		}

		DamageToTargets.AddUnique(OverlapActor);
	}
}

void UmsGameplayAbility_TeslaChain::AvataActorDied(AActor* DeadActor)
{
	GetAvatarActorFromActorInfo()->GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	RemoveElectrocuteCue(DeadActor);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UmsGameplayAbility_TeslaChain::ElectrocuteTargetDied(AActor* DeadActor)
{
	RemoveElectrocuteCue(DeadActor);
	RemoveElectrocuteEffectFromTarget(DeadActor);
}

void UmsGameplayAbility_TeslaChain::ApplyElectrocuteEffect()
{
	for (auto Target : DamageToTargets)
	{
		if (Target.IsValid())
		{
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target.Get());
			const FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
			const FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(ElectocuteEffect, GetAbilityLevel(), ContextHandle);
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}
	}
}

void UmsGameplayAbility_TeslaChain::ClearAllElectrocuteEffects()
{
	for (auto Target : DamageToTargets)
	{
		if (Target.IsValid())
		{
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target.Get());
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(FmsGameplayTags::Get().Debuff_Lightning);
			TargetASC->RemoveActiveEffectsWithGrantedTags(TagContainer);
		}
	}
}

void UmsGameplayAbility_TeslaChain::RemoveElectrocuteEffectFromTarget(AActor* Target)
{
	if (IsValid(Target))
	{
		if (DamageToTargets.Find(Target))
		{
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(FmsGameplayTags::Get().Debuff_Lightning);
			TargetASC->RemoveActiveEffectsWithGrantedTags(TagContainer);

			DamageToTargets.Remove(Target);
		}
	}
}

void UmsGameplayAbility_TeslaChain::ExpiredElectocute()
{
	GetAvatarActorFromActorInfo()->GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UmsGameplayAbility_TeslaChain::ApplyEletrocuteDamage()
{
	UAbilitySystemComponent* SoureASC = GetAbilitySystemComponentFromActorInfo();
	if (SourceActor.IsValid())
	{
		SoureASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor.Get());
	}

	FDamageEffectParams Params = MakeDamageEffectParamsFromClassDefault(GetAvatarActorFromActorInfo());
	Params.SourceAbilitySystemComponent = SoureASC;
	UmsAbilitySystemLibrary::ApplyDebuffDamage(Params);

	TArray<TWeakObjectPtr<AActor>> Targets = DamageToTargets;
	for (auto Target : Targets)
	{
		if (Target.IsValid())
		{
			AActor* TargetActor = Target.Get();
			if (IsValid(TargetActor))
			{
				Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
				UmsAbilitySystemLibrary::ApplyDebuffDamage(Params);
			}
		}
	}
}
