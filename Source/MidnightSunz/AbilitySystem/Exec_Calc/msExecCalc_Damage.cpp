// Fill out your copyright notice in the Description page of Project Settings.


#include "msExecCalc_Damage.h"
#include "Kismet/GameplayStatics.h"
#include "MidnightSunz/msGameplayTags.h"
#include "MidnightSunz/AbilitySystem/Attributes/msAttributeSet.h"
#include "MidnightSunz/AbilitySystem/msAbilitySystemLibrary.h"
#include "MidnightSunz/Interface/msCombatInterface.h"
#include "MidnightSunz/Interface/msArtifactInterface.h"

struct msDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);

	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(IceResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PoisonResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	msDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UmsAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UmsAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UmsAttributeSet, CriticalHitDamage, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UmsAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UmsAttributeSet, IceResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UmsAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UmsAttributeSet, PoisonResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UmsAttributeSet, PhysicalResistance, Target, false);
	}
};

static const msDamageStatics& DamageStatics()
{
	static msDamageStatics DamageStatics;
	return DamageStatics;
}

UmsExecCalc_Damage::UmsExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);

	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().IceResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PoisonResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}

void UmsExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	const FmsGameplayTags& Tags = FmsGameplayTags::Get();

	TagsToCaptureDefs.Add(Tags.Attributes_Combat_Armor, DamageStatics().ArmorDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Combat_CriticalHitChance, DamageStatics().CriticalHitChanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Combat_CriticalHitDamage, DamageStatics().CriticalHitDamageDef);

	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fire, DamageStatics().FireResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Ice, DamageStatics().IceResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Lightning, DamageStatics().LightningResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Poison, DamageStatics().PoisonResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Physical, DamageStatics().PhysicalResistanceDef);

	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

	// 차단
	const bool bBlockedDamage = TargetASC->GetAvatarActor()->Implements<UmsArtifactInterface>() ? ImsArtifactInterface::Execute_CheckProbability(TargetASC->GetAvatarActor(), Tags.Artifact_Glass) : false;
	if (bBlockedDamage)
	{
		UmsAbilitySystemLibrary::SetIsBlockedDamage(EffectContextHandle, true);

		const FGameplayModifierEvaluatedData EvaluatedData(UmsAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, 0.f);
		OutExecutionOutput.AddOutputModifier(EvaluatedData);
		return;
	}

	// 디버프
	DetermineDebuff(ExecutionParams, Spec, EvaluateParameters, TagsToCaptureDefs);

	// 데미지 계산

	UE_LOG(LogTemp, Log, TEXT("Starting damage type to resistance iteration"));
	float Damage = 0.f;
	for (const auto& Pair : Tags.DamageTypesToResistances)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag ResistanceTag = Pair.Value;

		UE_LOG(LogTemp, Log, TEXT("Processing DamageTypeTag: %s, ResistanceTag: %s"), *DamageTypeTag.ToString(), *ResistanceTag.ToString());
		if (!ensure(TagsToCaptureDefs.Contains(ResistanceTag)))
		{
			UE_LOG(LogTemp, Error, TEXT("TagsToCaptureDefs doesn't contain Tag: [%s]"), *ResistanceTag.ToString());
			continue;
		}

		const FGameplayEffectAttributeCaptureDefinition CaptureDef = TagsToCaptureDefs[ResistanceTag];
		float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key, false);
		if (DamageTypeValue <= 0.f)
		{
			continue;
		}

		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluateParameters, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

		// 데미지 저항
		DamageTypeValue *= (100.f - Resistance) / 100.f;

		if (UmsAbilitySystemLibrary::IsRadialDamage(EffectContextHandle))
		{
			AActor* TargetAvata = TargetASC->GetAvatarActor();
			if (ImsCombatInterface* CombatInterface = Cast<ImsCombatInterface>(TargetAvata))
			{
				CombatInterface->GetOnDamageSignature().AddLambda([&](float DamageAmount)
					{
						DamageTypeValue = DamageAmount;
					});
			}

			TArray<AActor*> IgnoreActors;
			AActor* SourceAvata = SourceASC->GetAvatarActor();
			IgnoreActors.Add(SourceAvata);
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				TargetAvata,
				DamageTypeValue,
				0.f,
				UmsAbilitySystemLibrary::GetRadialDamageOrigin(EffectContextHandle),
				UmsAbilitySystemLibrary::GetRadialDamageInnerRadius(EffectContextHandle),
				UmsAbilitySystemLibrary::GetRadialDamageOuterRadius(EffectContextHandle),
				1.f,
				UDamageType::StaticClass(),
				IgnoreActors,
				SourceAvata,
				nullptr);
		}

		Damage += DamageTypeValue;
	}

	// 방어력 
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TagsToCaptureDefs[Tags.Attributes_Combat_Armor], EvaluateParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.f);

	Damage *= (100.f - TargetArmor) / 100.f;

	// 크리티컬
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TagsToCaptureDefs[Tags.Attributes_Combat_CriticalHitChance], EvaluateParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);

	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TagsToCaptureDefs[Tags.Attributes_Combat_CriticalHitDamage], EvaluateParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.f);

	const bool bCriticalHit = FMath::RandRange(1, 100) < SourceCriticalHitChance;
	if (bCriticalHit)
	{
		Damage = Damage * (1 + SourceCriticalHitDamage / 100.f);
	}

	UmsAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);

	const FGameplayModifierEvaluatedData EvaluatedData(UmsAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}

void UmsExecCalc_Damage::DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectSpec& Spec, FAggregatorEvaluateParameters& EvaluateParameters, const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& InTagsToDefs) const
{
	const FmsGameplayTags& GameplayTags = FmsGameplayTags::Get();
	for (const auto& Pair : GameplayTags.DamageTypesToDebuffs)
	{
		const FGameplayTag& DamageType = Pair.Key;
		const FGameplayTag& DebuffType = Pair.Value;
		const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageType, false, -1.f);
		if (TypeDamage > -.5f)
		{
			const float SourceDebuffChance = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Chance, false, -1.f);

			float TargetDebuffResistance = 0.f;
			const FGameplayTag& ResistanceTag = GameplayTags.DamageTypesToResistances[DamageType];
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(InTagsToDefs[ResistanceTag], EvaluateParameters, TargetDebuffResistance);
			TargetDebuffResistance = FMath::Max<float>(TargetDebuffResistance, 0.f);

			const float EffectiveDebuffChance = SourceDebuffChance * (100.f - TargetDebuffResistance) / 100.f;
			const bool bDebuff = FMath::RandRange(1, 100) < EffectiveDebuffChance;
			if (bDebuff)
			{
				FGameplayEffectContextHandle ContextHandle = Spec.GetContext();

				UmsAbilitySystemLibrary::SetIsSuccessfulDebuff(ContextHandle, true);
				UmsAbilitySystemLibrary::SetDamageType(ContextHandle, DamageType);

				const float DebuffDamage = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Damage, false, -1.f);
				const float DebuffDuration = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Duration, false, -1.f);
				const float DebuffFrequency = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Frequency, false, -1.f);

				UmsAbilitySystemLibrary::SetDebuffDamage(ContextHandle, DebuffDamage);
				UmsAbilitySystemLibrary::SetDebuffDuration(ContextHandle, DebuffDuration);
				UmsAbilitySystemLibrary::SetDebuffFrequency(ContextHandle, DebuffFrequency);
			}
		}

	}
}