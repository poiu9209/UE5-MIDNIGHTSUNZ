// Fill out your copyright notice in the Description page of Project Settings.


#include "msGameplayTags.h"
#include "GameplayTagsManager.h"

FmsGameplayTags FmsGameplayTags::GameplayTags;

void FmsGameplayTags::InitializeNativeGameplayTags()
{
	/* Attributes */
	GameplayTags.Attributes_Vital_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Vital.MaxHealth"), FString("Max Health"));
	GameplayTags.Attributes_Combat_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Combat.Armor"), FString("Damage reduction"));
	GameplayTags.Attributes_Combat_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Combat.CriticalHitChance"), FString("CriticalHitChance"));
	GameplayTags.Attributes_Combat_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Combat.CriticalHitDamage"), FString("CriticalHitDamage"));

	/* Input Tags */
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.LMB"), FString("Input Tag for Left Mouse Button"));
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.RMB"), FString("Input Tag for Right Mouse Button"));
	GameplayTags.InputTag_Space = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Space"), FString("Input Tag for Space key"));
	GameplayTags.InputTag_Shift = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Shift"), FString("Input Tag for Shift Key"));
	GameplayTags.InputTag_E = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.E"), FString("Input Tag for E Key"));
	GameplayTags.InputTag_Q = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Q"), FString("Input Tag for Q Key"));
	GameplayTags.InputTag_Tab = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("InputTag.Tab"), FString("Input Tag for Tab Key"));

	GameplayTags.SkillAbilityTags.AddUnique(GameplayTags.InputTag_LMB);
	GameplayTags.SkillAbilityTags.AddUnique(GameplayTags.InputTag_RMB);
	GameplayTags.SkillAbilityTags.AddUnique(GameplayTags.InputTag_Space);
	GameplayTags.SkillAbilityTags.AddUnique(GameplayTags.InputTag_Shift);
	GameplayTags.SkillAbilityTags.AddUnique(GameplayTags.InputTag_E);
	GameplayTags.SkillAbilityTags.AddUnique(GameplayTags.InputTag_Q);

	/* Damage Types */
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage"), FString("Damage"));
	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Fire"), FString("Fire DamageType"));
	GameplayTags.Damage_Ice = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Ice"), FString("Ice DamageType"));
	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Lightning"), FString("Lightning DamageType"));
	GameplayTags.Damage_Poison = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Poison"), FString("Poison DamageType"));
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Physical"), FString("Physical DamageType"));

	/* DamageType Resistance */
	GameplayTags.Attributes_Resistance_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistance.Fire"), FString("Resistance to Fire damage"));
	GameplayTags.Attributes_Resistance_Ice = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistance.Ice"), FString("Resistance to Ice damage"));
	GameplayTags.Attributes_Resistance_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistance.Lightning"), FString("Resistance to Lightning damage"));
	GameplayTags.Attributes_Resistance_Poison = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistance.Poison"), FString("Resistance to Poison damage"));
	GameplayTags.Attributes_Resistance_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistance.Physical"), FString("Resistance to Physical damage"));

	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Fire, GameplayTags.Attributes_Resistance_Fire);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Ice, GameplayTags.Attributes_Resistance_Ice);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Lightning, GameplayTags.Attributes_Resistance_Lightning);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Poison, GameplayTags.Attributes_Resistance_Poison);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Physical, GameplayTags.Attributes_Resistance_Physical);

	/* Buff */
	GameplayTags.Buff_ReduceCooldown = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Buff.ReduceCooldown"), FString("Reduce Cooldown Buff Tag"));
	GameplayTags.Buff_IncreaseArmor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Buff.IncreaseArmor"), FString("Increase Armor Buff Tag"));
	GameplayTags.Buff_IncreaseCriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Buff.IncreaseCriticalHitChance"), FString("Increase CriticalHitChance Buff Tag"));
	GameplayTags.Buff_IncreaseCriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Buff.IncreaseCriticalHitDamage"), FString("Increase CriticalHitDamage Buff Tag"));
	GameplayTags.Buff_IncreaseResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Buff.IncreaseResistance"), FString("Increase Resistance Buff Tag"));

	GameplayTags.BuffTags.Add(GameplayTags.Buff_ReduceCooldown);
	GameplayTags.BuffTags.Add(GameplayTags.Buff_IncreaseArmor);
	GameplayTags.BuffTags.Add(GameplayTags.Buff_IncreaseCriticalHitChance);
	GameplayTags.BuffTags.Add(GameplayTags.Buff_IncreaseCriticalHitDamage);
	GameplayTags.BuffTags.Add(GameplayTags.Buff_IncreaseResistance);

	/* Debuffs */
	GameplayTags.Debuff_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Fire"), FString("Fire Debuff"));
	GameplayTags.Debuff_Ice = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Ice"), FString("Ice Debuff"));
	GameplayTags.Debuff_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Lightning"), FString("Lightning Debuff"));
	GameplayTags.Debuff_Poison = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Poison"), FString("Poison Debuff"));

	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Fire, GameplayTags.Debuff_Fire);
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Lightning, GameplayTags.Debuff_Lightning);
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Poison, GameplayTags.Debuff_Poison);
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Ice, GameplayTags.Debuff_Ice);
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Physical, GameplayTags.Debuff_Physical);

	GameplayTags.Debuff_Chance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Chance"), FString("Debuff Chance"));
	GameplayTags.Debuff_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Damage"), FString("Debuff Damage"));
	GameplayTags.Debuff_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Duration"), FString("Debuff Duration"));
	GameplayTags.Debuff_Frequency = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Frequency"), FString("Debuff Frequency"));

	/* Abilities */
	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Attack"), FString("Attack Ability Tag"));
	GameplayTags.Abilities_Attack_Melee = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Attack.Melee"), FString("Melee Attack Ability Tag"));
	GameplayTags.Abilities_Attack_Ranged = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Attack.Ranged"), FString("Ranged Attack Ability Tag"));
	GameplayTags.Abilities_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.Summon"), FString("Summon Ability Tag"));
	GameplayTags.Abilities_TeslaChain = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Abilities.TeslaChain"), FString("Tesla Chain Ability Tag"));

	/* Artifact */
	GameplayTags.Artifact_ChickenLeg = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Artifact.ChickenLeg"), FString("ChickenLeg Artifact Tag"));
	GameplayTags.Artifact_BlueDrug = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Artifact.BlueDrug"), FString("BlueDrug Artifact Tag"));
	GameplayTags.Artifact_CeremonialDagger = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Artifact.CeremonialDagger"), FString("CeremonialDagger Artifact Tag"));
	GameplayTags.Artifact_PoisonApple = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Artifact.PoisonApple"), FString("PoisonApple Artifact Tag"));
	GameplayTags.Artifact_IceMushroom = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Artifact.IceMushroom"), FString("IceMushroom Artifact Tag"));
	GameplayTags.Artifact_ArcReactor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Artifact.ArcReactor"), FString("ArcReactor Artifact Tag"));
	GameplayTags.Artifact_RedDrug = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Artifact.RedDrug"), FString("RedDrug Artifact Tag"));
	GameplayTags.Artifact_Adrenaline = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Artifact.Adrenaline"), FString("Adrenaline Artifact Tag"));
	GameplayTags.Artifact_RandomBox = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Artifact.RandomBox"), FString("RandomBox Artifact Tag"));
	GameplayTags.Artifact_Cannie = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Artifact.Cannie"), FString("Cannie Artifact Tag"));
	GameplayTags.Artifact_Glass = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Artifact.Glass"), FString("Glass Artifact Tag"));

	GameplayTags.ArtifactTags.Add(GameplayTags.Artifact_ChickenLeg);
	GameplayTags.ArtifactTags.Add(GameplayTags.Artifact_BlueDrug);
	GameplayTags.ArtifactTags.Add(GameplayTags.Artifact_CeremonialDagger);
	GameplayTags.ArtifactTags.Add(GameplayTags.Artifact_PoisonApple);
	GameplayTags.ArtifactTags.Add(GameplayTags.Artifact_IceMushroom);
	GameplayTags.ArtifactTags.Add(GameplayTags.Artifact_ArcReactor);
	GameplayTags.ArtifactTags.Add(GameplayTags.Artifact_RedDrug);
	GameplayTags.ArtifactTags.Add(GameplayTags.Artifact_Adrenaline);
	GameplayTags.ArtifactTags.Add(GameplayTags.Artifact_RandomBox);
	GameplayTags.ArtifactTags.Add(GameplayTags.Artifact_Cannie);
	GameplayTags.ArtifactTags.Add(GameplayTags.Artifact_Glass);

	/* Colldown */
	GameplayTags.Cooldown_Skill_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Cooldown.Skill.Fire"), FString("Fire Cooldown Tag"));
	GameplayTags.Cooldown_Skill_Dash = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Cooldown.Skill.Dash"), FString("Dash Cooldown Tag"));

	/* Combat Socket */
	GameplayTags.CombatSocket_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CombatSocket.Weapon"), FString("Weapon"));
	GameplayTags.CombatSocket_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CombatSocket.RightHand"), FString("Right Hand"));
	GameplayTags.CombatSocket_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CombatSocket.LeftHand"), FString("Left Hand"));

	/* Montage Tags */
	GameplayTags.Montage_Attack_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.1"), FString("Attack 1"));
	GameplayTags.Montage_Attack_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.2"), FString("Attack 2"));
	GameplayTags.Montage_Attack_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.3"), FString("Attack 3"));
	GameplayTags.Montage_Attack_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Montage.Attack.4"), FString("Attack 4"));

	/* Effects */
	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Effects.HitReact"), FString("Effects_HitReact"));

	/* Player */
	GameplayTags.Player_Block_InputPressed = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Player.Block.InputPressed"), FString("Block InputPressed"));
	GameplayTags.Player_Block_InputHeld = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Player.Block.InputHeld"), FString("Block InputHeld"));
	GameplayTags.Player_Block_InputReleased = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Player.Block.InputReleased"), FString("Block InputReleased"));
	GameplayTags.Player_Block_TraceCurosr = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Player.Block.TraceCurosr"), FString("Block TraceCurosr"));

	GameplayTags.GameplayCue_ArrowImpact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.ArrowImpact"), FString("ArrowImpact GameplayCue Tag"));

	GameplayTags.SetByCaller_Cooldown = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("SetByCaller.Cooldown"), FString("SetByCaller Cooldown Tag"));
}
