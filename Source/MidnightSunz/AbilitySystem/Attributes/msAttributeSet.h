// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "msAttributeSet.generated.h"

struct FDamageTextParams;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT(BlueprintType)
struct FEffectProperties
{
	GENERATED_BODY()

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> SourceAvataActor = nullptr;

	UPROPERTY()
	TObjectPtr<AController> SourceController = nullptr;

	UPROPERTY()
	TObjectPtr<ACharacter> SourceCharacter = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetASC = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> TargetAvataActor = nullptr;

	UPROPERTY()
	TObjectPtr<AController> TargetController = nullptr;

	UPROPERTY()
	TObjectPtr<ACharacter> TargetCharacter = nullptr;
};

//typedef TBaseStaticDelegateInstance<FGameplayAttribute(), FDefaultDelegateUserPolicy>::FFuncPtr FAttributeFuncPtr;
template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API UmsAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UmsAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UmsAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UmsAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Combat Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UmsAttributeSet, Armor);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitChance, Category = "Combat Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CriticalHitChance;
	ATTRIBUTE_ACCESSORS(UmsAttributeSet, CriticalHitChance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitDamage, Category = "Combat Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CriticalHitDamage;
	ATTRIBUTE_ACCESSORS(UmsAttributeSet, CriticalHitDamage);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MovementSpeedIncrease, Category = "Combat Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MovementSpeedIncrease;
	ATTRIBUTE_ACCESSORS(UmsAttributeSet, MovementSpeedIncrease);

	/* Resistance */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireResistance, Category = "Resistance Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData FireResistance;
	ATTRIBUTE_ACCESSORS(UmsAttributeSet, FireResistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IceResistance, Category = "Resistance Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData IceResistance;
	ATTRIBUTE_ACCESSORS(UmsAttributeSet, IceResistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningResistance, Category = "Resistance Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData LightningResistance;
	ATTRIBUTE_ACCESSORS(UmsAttributeSet, LightningResistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PoisonResistance, Category = "Resistance Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData PoisonResistance;
	ATTRIBUTE_ACCESSORS(UmsAttributeSet, PoisonResistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalResistance, Category = "Resistance Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData PhysicalResistance;
	ATTRIBUTE_ACCESSORS(UmsAttributeSet, PhysicalResistance);

	// -------------------------------------------------------------------
	//	Meta Attribute (please keep attributes that aren't 'stateful' below 
	// -------------------------------------------------------------------

	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UmsAttributeSet, Damage);

	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;

private:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_CriticalHitChance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MovementSpeedIncrease(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_FireResistance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_IceResistance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_LightningResistance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_PoisonResistance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_PhysicalResistance(const FGameplayAttributeData& OldValue);

private:
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
	void ExtractProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& OutProperties) const;
	void ShowDamageText(const FEffectProperties& Properties, const FDamageTextParams& DamageTextParams);
	void HandleDamage(const FEffectProperties& Properties);
	void HandleDebuff(const FEffectProperties& Properties);
	void ApplyPhysicalDebuff(const FEffectProperties& Properties);
	void ApplyIceDebuff(const FGameplayTag& DamageType, const FEffectProperties& Properties);
	void ApplyLightingDebuff(const FEffectProperties& Properties);
	void ApplyElemetalDamageDebuff(const FGameplayTag& DamageType, const FEffectProperties& Properties);
};
