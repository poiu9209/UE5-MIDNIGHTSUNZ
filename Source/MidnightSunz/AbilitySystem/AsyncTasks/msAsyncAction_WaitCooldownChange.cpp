// Fill out your copyright notice in the Description page of Project Settings.


#include "msAsyncAction_WaitCooldownChange.h"
#include "AbilitySystemComponent.h"

UmsAsyncAction_WaitCooldownChange* UmsAsyncAction_WaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* InASC, const FGameplayTag& InCooldownTag, const bool bInCountChange /*= false*/)
{
	UmsAsyncAction_WaitCooldownChange* WaitCooldownChange = NewObject<UmsAsyncAction_WaitCooldownChange>();
	WaitCooldownChange->ASC = InASC;
	WaitCooldownChange->CooldownTag = InCooldownTag;
	WaitCooldownChange->bCountChange = bInCountChange;

	if (!IsValid(InASC) || !InCooldownTag.IsValid())
	{
		WaitCooldownChange->EndTask();
		return nullptr;
	}

	if (bInCountChange)
	{
		// 버프류는 중첩될때마다 갱신 
		InASC->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::AnyCountChange).AddUObject(WaitCooldownChange, &UmsAsyncAction_WaitCooldownChange::CooldownTagChanged);

	}
	else
	{
		// 쿨다운 끝남 (태그 제거될때)
		InASC->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(WaitCooldownChange, &UmsAsyncAction_WaitCooldownChange::CooldownTagChanged);
	}

	// 쿨다운 시작 ( 쿨다운 GE 적용될때)
	InASC->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange, &UmsAsyncAction_WaitCooldownChange::OnActiveEffectAdded);

	return WaitCooldownChange;
}

void UmsAsyncAction_WaitCooldownChange::EndTask()
{
	if (!IsValid(ASC))
	{
		return;
	}

	ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::AnyCountChange).RemoveAll(this);
	ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	SetReadyToDestroy();
	MarkAsGarbage();
}

float UmsAsyncAction_WaitCooldownChange::GetTimeRemaining() const
{
	FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
	TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
	if (TimesRemaining.Num() > 0)
	{
		float TimeRemaining = TimesRemaining[0];
		for (int32 i = 0; i < TimesRemaining.Num(); i++)
		{
			if (TimesRemaining[i] > TimeRemaining)
			{
				TimeRemaining = TimesRemaining[i];
			}
		}

		return TimeRemaining;
	}

	return 0.0f;
}

void UmsAsyncAction_WaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		CooldownEnd.Broadcast(0.f);
		return;
	}

	if (bCountChange)
	{
		HandleCooldownStart();
	}
}

void UmsAsyncAction_WaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GratedTags;
	SpecApplied.GetAllGrantedTags(GratedTags);

	if (AssetTags.HasTagExact(CooldownTag) || GratedTags.HasTagExact(CooldownTag))
	{
		HandleCooldownStart();
	}
}

void UmsAsyncAction_WaitCooldownChange::HandleCooldownStart()
{
	FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
	TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
	if (TimesRemaining.Num() > 0)
	{
		float TimeRemaining = TimesRemaining[0];
		for (int32 i = 0; i < TimesRemaining.Num(); i++)
		{
			if (TimesRemaining[i] > TimeRemaining)
			{
				TimeRemaining = TimesRemaining[i];
			}
		}

		CooldownStart.Broadcast(TimeRemaining);
	}
}
