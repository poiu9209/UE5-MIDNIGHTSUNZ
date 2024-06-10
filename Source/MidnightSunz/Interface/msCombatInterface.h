// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "ActiveGameplayEffectHandle.h"
#include "msCombatInterface.generated.h"

class USkeletalMeshComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDamageSignature, float /*DamageAmount*/);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, AActor*, DeadActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBuffChangeSignature, const FGameplayTag&, BuffTag, int32, Count);

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	Melee,
	Ranged,
	Summon,
};

USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EAttackType AttackType = EAttackType::Melee;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag MontageTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag SocketTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* ImpactSound = nullptr;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UmsCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class MIDNIGHTSUNZ_API ImsCombatInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USkeletalMeshComponent* GetWeapon() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetPlayerLevel();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetCombatSocketLocation(const FGameplayTag& MontageTag) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateFacingTarget(const FVector& Target);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateMotionWarpTarget(const FVector& Target);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowDamageText(const FDamageTextParams& Params);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontate() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BP_Die();

	virtual void Die() = 0;
	virtual FOnDamageSignature& GetOnDamageSignature() = 0;
	virtual FOnDeathSignature& GetOnDeathDelegate() = 0;
	virtual FOnBuffChangeSignature& GetOnBuffChangeDelegate() = 0;

	virtual bool IsFullHealth() = 0;

	virtual void GetBuffActiveEffectHandles(const FGameplayTag& InTag, TArray<FActiveGameplayEffectHandle>& OutHandles) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetCombatTarget(AActor* InCombatTarget);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AActor* GetCombatTarget() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAvatar();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FTaggedMontage> GetAttackMontages(EAttackType AttackType) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UNiagaraSystem* GetBloodEffect() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FTaggedMontage GetTaggedMontageByTag(const FGameplayTag& MontageTag) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetSummonedActorCount() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddSummonedActor(AActor* InActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsBeingElectrocuted() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetIsBeingElectrocuted(bool bInElectrocute);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetMaxHealth() const;
};
