// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * Native로 추가한 게임플레이 태그
 */

struct FmsGameplayTags
{
public:
	static FmsGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	/* Attribute */
	FGameplayTag Attributes_Vital_MaxHealth;
	FGameplayTag Attributes_Combat_Armor;
	FGameplayTag Attributes_Combat_CriticalHitChance;
	FGameplayTag Attributes_Combat_CriticalHitDamage;

	/* Input */
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_Shift;
	FGameplayTag InputTag_Space;
	FGameplayTag InputTag_E;
	FGameplayTag InputTag_Q;
	FGameplayTag InputTag_Tab;

	/* Dmage */
	FGameplayTag Damage;
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Ice;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Poison;
	FGameplayTag Damage_Physical;

	/* Resistance */
	FGameplayTag Attributes_Resistance_Fire;
	FGameplayTag Attributes_Resistance_Ice;
	FGameplayTag Attributes_Resistance_Lightning;
	FGameplayTag Attributes_Resistance_Poison;
	FGameplayTag Attributes_Resistance_Physical;

	/* Buff */
	FGameplayTag Buff_ReduceCooldown;
	FGameplayTag Buff_IncreaseArmor;
	FGameplayTag Buff_IncreaseCriticalHitChance;
	FGameplayTag Buff_IncreaseCriticalHitDamage;
	FGameplayTag Buff_IncreaseResistance;

	/* Debuff */
	FGameplayTag Debuff_Fire;
	FGameplayTag Debuff_Ice;
	FGameplayTag Debuff_Lightning;
	FGameplayTag Debuff_Poison;
	FGameplayTag Debuff_Physical;

	FGameplayTag Debuff_Chance;
	FGameplayTag Debuff_Damage;
	FGameplayTag Debuff_Duration;
	FGameplayTag Debuff_Frequency;

	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_Attack_Melee;
	FGameplayTag Abilities_Attack_Ranged;
	FGameplayTag Abilities_Summon;
	FGameplayTag Abilities_TeslaChain;

	/* Artifact */
	FGameplayTag Artifact_ChickenLeg;
	FGameplayTag Artifact_BlueDrug;
	FGameplayTag Artifact_CeremonialDagger;
	FGameplayTag Artifact_PoisonApple;
	FGameplayTag Artifact_IceMushroom;
	FGameplayTag Artifact_ArcReactor;
	FGameplayTag Artifact_RedDrug;
	FGameplayTag Artifact_Adrenaline;
	FGameplayTag Artifact_RandomBox;
	FGameplayTag Artifact_Cannie;
	FGameplayTag Artifact_Glass;

	/* Cooldown */
	FGameplayTag Cooldown_Skill_Fire;
	FGameplayTag Cooldown_Skill_Dash;

	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_LeftHand;

	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;

	FGameplayTag Effects_HitReact;

	/* Player */
	FGameplayTag Player_Block_InputPressed;
	FGameplayTag Player_Block_InputHeld;
	FGameplayTag Player_Block_InputReleased;
	FGameplayTag Player_Block_TraceCurosr;

	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;
	TMap<FGameplayTag, FGameplayTag> DamageTypesToDebuffs;

	TArray<FGameplayTag> SkillAbilityTags;
	TArray<FGameplayTag> BuffTags;
	TArray<FGameplayTag> ArtifactTags;

	FGameplayTag GameplayCue_ArrowImpact;

	FGameplayTag SetByCaller_Cooldown;

private:
	static FmsGameplayTags GameplayTags;
};