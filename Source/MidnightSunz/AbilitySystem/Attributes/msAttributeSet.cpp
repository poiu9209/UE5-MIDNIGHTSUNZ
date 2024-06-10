// Fill out your copyright notice in the Description page of Project Settings.


#include "msAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "MidnightSunz/msGameplayTags.h"
#include "MidnightSunz/msGameplayEffectContext.h"
#include "MidnightSunz/Player/msPlayerController.h"
#include "MidnightSunz/Interface/msCombatInterface.h"
#include "MidnightSunz/Interface/msArtifactInterface.h"
#include "MidnightSunz/AbilitySystem/msAbilitySystemLibrary.h"
#include "MidnightSunz/UI/msUITypes.h"

UmsAttributeSet::UmsAttributeSet()
{
	const FmsGameplayTags& GameplayTags = FmsGameplayTags::Get();

	TagsToAttributes.Add(GameplayTags.Attributes_Vital_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Combat_Armor, GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Combat_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Combat_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Ice, GetIceResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Lightning, GetLightningResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Poison, GetPoisonResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);
}

void UmsAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UmsAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UmsAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UmsAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UmsAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UmsAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UmsAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UmsAttributeSet, IceResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UmsAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UmsAttributeSet, PoisonResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UmsAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UmsAttributeSet, MovementSpeedIncrease, COND_None, REPNOTIFY_Always);
}

void UmsAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UmsAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		SetHealth(GetMaxHealth());
	}
}

void UmsAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
}

void UmsAttributeSet::ExtractProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& OutProperties) const
{
	OutProperties.EffectContextHandle = Data.EffectSpec.GetContext();
	OutProperties.SourceASC = OutProperties.EffectContextHandle.GetInstigatorAbilitySystemComponent();

	if (IsValid(OutProperties.SourceASC) && OutProperties.SourceASC->AbilityActorInfo.IsValid() && OutProperties.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		OutProperties.SourceAvataActor = OutProperties.SourceASC->AbilityActorInfo->AvatarActor.Get();
		OutProperties.SourceController = OutProperties.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (!OutProperties.SourceController && OutProperties.SourceAvataActor)
		{
			if (const APawn* Pawn = Cast<APawn>(OutProperties.SourceAvataActor))
			{
				OutProperties.SourceController = Pawn->GetController();
			}
		}
		if (OutProperties.SourceController)
		{
			OutProperties.SourceCharacter = Cast<ACharacter>(OutProperties.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		OutProperties.TargetAvataActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		OutProperties.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		OutProperties.TargetCharacter = Cast<ACharacter>(OutProperties.TargetAvataActor);
		OutProperties.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OutProperties.TargetAvataActor);
	}
}

void UmsAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Properties;
	ExtractProperties(Data, Properties);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		HandleDamage(Properties);
	}
}

void UmsAttributeSet::HandleDamage(const FEffectProperties& Properties)
{
	const float LocalDmamage = GetDamage();
	SetDamage(0.f);

	if (GetHealth() <= 0.f)
	{
		return;
	}

	if (UmsAbilitySystemLibrary::IsBlockedDamage(Properties.EffectContextHandle))
	{
		FDamageTextParams Params;
		Params.bIsBlockedDamage = true;
		ShowDamageText(Properties, Params);
		return;
	}

	const bool bIsSourceCombat = Properties.SourceAvataActor->Implements<UmsCombatInterface>();
	const bool bIsSourceDead = bIsSourceCombat ? ImsCombatInterface::Execute_IsDead(Properties.SourceAvataActor) : false;
	const bool bIsSourceArtifact = Properties.SourceAvataActor->Implements<UmsArtifactInterface>();

	if (LocalDmamage > 0.f)
	{
		const float NewHealth = GetHealth() - LocalDmamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

		const bool bDie = NewHealth <= 0.f;
		if (bDie)
		{
			if (Properties.TargetAvataActor->Implements<UmsCombatInterface>() && !ImsCombatInterface::Execute_IsDead(Properties.TargetAvataActor))
			{
				if (bIsSourceCombat && !bIsSourceDead && bIsSourceArtifact)
				{
					ImsArtifactInterface::Execute_TriggerKillEnemyArtifact(Properties.SourceAvataActor, Properties.TargetAvataActor->GetActorLocation());
				}
				Cast<ImsCombatInterface>(Properties.TargetAvataActor)->Die();
			}
		}

		const bool bCiriticalHit = UmsAbilitySystemLibrary::IsCriticalHit(Properties.EffectContextHandle);
		const FGameplayTag DamageType = UmsAbilitySystemLibrary::GetDamageType(Properties.EffectContextHandle);
		FDamageTextParams Params(DamageType, LocalDmamage, bCiriticalHit);

		ShowDamageText(Properties, Params);
	}

	const bool bSuccessDebuff = UmsAbilitySystemLibrary::IsSuccessfulDebuff(Properties.EffectContextHandle);
	if (bSuccessDebuff)
	{
		HandleDebuff(Properties);
	}

	const bool CanTriggerHitEnemyArtifact = !UmsAbilitySystemLibrary::IsArtifactDamage(Properties.EffectContextHandle) && !bSuccessDebuff && bIsSourceCombat && !bIsSourceDead && bIsSourceArtifact;
	if (CanTriggerHitEnemyArtifact)
	{
		ImsArtifactInterface::Execute_TriggerHitEnemyArtifact(Properties.SourceAvataActor, Properties.TargetAvataActor);
	}
}

void UmsAttributeSet::HandleDebuff(const FEffectProperties& Properties)
{
	const FmsGameplayTags& GameplayTags = FmsGameplayTags::Get();
	const FGameplayTag DamageType = UmsAbilitySystemLibrary::GetDamageType(
		Properties.EffectContextHandle);

	if (DamageType == GameplayTags.Damage_Physical)
	{
		ApplyPhysicalDebuff(Properties);
	}
	else if (DamageType == GameplayTags.Damage_Ice)
	{
		if (!Properties.TargetASC->HasMatchingGameplayTag(
			FmsGameplayTags::Get().Debuff_Ice))
		{
			ApplyIceDebuff(DamageType, Properties);
		}
	}
	else if (DamageType == GameplayTags.Damage_Lightning)
	{
		if (!Properties.TargetASC->HasMatchingGameplayTag(
			FmsGameplayTags::Get().Debuff_Lightning))
		{
			ApplyLightingDebuff(Properties);
		}
	}
	else
	{
		ApplyElemetalDamageDebuff(DamageType, Properties);
	}
}

void UmsAttributeSet::ApplyPhysicalDebuff(const FEffectProperties& Properties)
{
	// 물리 휘청거림

	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(FmsGameplayTags::Get().Effects_HitReact);
	Properties.TargetASC->TryActivateAbilitiesByTag(TagContainer);
}

void UmsAttributeSet::ApplyIceDebuff(const FGameplayTag& DamageType, const FEffectProperties& Properties)
{
	// 냉기 속도감소

	// Dynamic GameplayEffect 
	// 뵥제 지원안된다. 그러니 유의하자

	FGameplayEffectContextHandle EffectContext = Properties.SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(Properties.SourceAvataActor);

	const float DebuffDuration = UmsAbilitySystemLibrary::GetDebuffDuration(Properties.EffectContextHandle);

	FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DamageType.ToString());
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));

	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Effect->Period = 0.f;
	Effect->DurationMagnitude = FScalableFloat(DebuffDuration);

	Effect->InheritableOwnedTagsContainer.AddTag(FmsGameplayTags::Get().DamageTypesToDebuffs[DamageType]);

	Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;
	Effect->StackLimitCount = 1;

	if (FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContext, 1.f))
	{
		FmsGameplayEffectContext* msContext = static_cast<FmsGameplayEffectContext*>(MutableSpec->GetContext().Get());
		TSharedPtr<FGameplayTag> DebuffDamageType = MakeShareable(new FGameplayTag(DamageType));
		msContext->SetDamageType(DebuffDamageType);
		// 아티팩트데미지인지 꼭 설정해야함, 무한루프에 빠질수있음.
		msContext->SetIsArtifactDamage(UmsAbilitySystemLibrary::IsArtifactDamage(Properties.EffectContextHandle));
		// 여기 디버프 담으면 무한루프에 빠질수있음, 주의

		Properties.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
	}
}

void UmsAttributeSet::ApplyLightingDebuff(const FEffectProperties& Properties)
{
	// 전기 주변적 감전
	if (FGameplayAbilitySpec* Spec = UmsAbilitySystemLibrary::GetSpecFromAbilityTag(Properties.TargetASC, FmsGameplayTags::Get().Abilities_TeslaChain))
	{
		Spec->SourceObject = Properties.SourceAvataActor;
		Properties.TargetASC->TryActivateAbility(Spec->Handle);
	}
}

void UmsAttributeSet::ApplyElemetalDamageDebuff(const FGameplayTag& DamageType, const FEffectProperties& Properties)
{
	// 불, 독 디버프는 데미지 틱 GE

	// Dynamic GameplayEffect 
	// 뵥제 지원안된다. 그러니 유의하자

	FGameplayEffectContextHandle EffectContext = Properties.SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(Properties.SourceAvataActor);

	const float DebuffDamage = UmsAbilitySystemLibrary::GetDebuffDamage(Properties.EffectContextHandle);
	const float DebuffDuration = UmsAbilitySystemLibrary::GetDebuffDuration(Properties.EffectContextHandle);
	const float DebuffFrequency = UmsAbilitySystemLibrary::GetDebuffFrequency(Properties.EffectContextHandle);

	FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DamageType.ToString());
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));

	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Effect->Period = DebuffFrequency;
	Effect->DurationMagnitude = FScalableFloat(DebuffDuration);

	Effect->InheritableOwnedTagsContainer.AddTag(FmsGameplayTags::Get().DamageTypesToDebuffs[DamageType]);

	Effect->StackingType = EGameplayEffectStackingType::AggregateByTarget;
	Effect->StackLimitCount = DamageType == FmsGameplayTags::Get().Damage_Fire ? 0 : 3;

	const int32 Index = Effect->Modifiers.Num();
	Effect->Modifiers.Add(FGameplayModifierInfo());
	FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Index];

	ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = UmsAttributeSet::GetDamageAttribute();

	if (FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContext, 1.f))
	{
		FmsGameplayEffectContext* msContext = static_cast<FmsGameplayEffectContext*>(MutableSpec->GetContext().Get());
		TSharedPtr<FGameplayTag> DebuffDamageType = MakeShareable(new FGameplayTag(DamageType));
		msContext->SetDamageType(DebuffDamageType);

		// 아티팩트데미지인지 꼭 설정해야함, 무한루프에 빠질수있음.
		msContext->SetIsArtifactDamage(UmsAbilitySystemLibrary::IsArtifactDamage(Properties.EffectContextHandle));

		// 여기 디버프 담으면 무한루프에 빠질수있음, 주의
		Properties.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
	}
}

void UmsAttributeSet::ShowDamageText(const FEffectProperties& Properties, const FDamageTextParams& DamageTextParams)
{
	if (Properties.SourceCharacter == Properties.TargetCharacter)
	{
		return;
	}

	AmsPlayerController* PC = Cast<AmsPlayerController>(Properties.SourceController);
	if (!PC)
	{
		PC = Cast<AmsPlayerController>(Properties.TargetController);
	}
	if (PC)
	{
		PC->ShowDamageNumber(Properties.TargetCharacter, DamageTextParams);
	}
}

void UmsAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UmsAttributeSet, Health, OldValue);
}

void UmsAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UmsAttributeSet, MaxHealth, OldValue);
}

void UmsAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UmsAttributeSet, Armor, OldValue);
}

void UmsAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UmsAttributeSet, CriticalHitChance, OldValue);
}

void UmsAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UmsAttributeSet, CriticalHitDamage, OldValue);
}

void UmsAttributeSet::OnRep_MovementSpeedIncrease(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UmsAttributeSet, MovementSpeedIncrease, OldValue);
}

void UmsAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UmsAttributeSet, FireResistance, OldValue);
}

void UmsAttributeSet::OnRep_IceResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UmsAttributeSet, IceResistance, OldValue);
}

void UmsAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UmsAttributeSet, LightningResistance, OldValue);
}

void UmsAttributeSet::OnRep_PoisonResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UmsAttributeSet, PoisonResistance, OldValue);
}

void UmsAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UmsAttributeSet, PhysicalResistance, OldValue);
}
