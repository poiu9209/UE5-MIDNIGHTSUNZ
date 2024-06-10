// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "MidnightSunz/Interface/msCombatInterface.h"
#include "MidnightSunz/Types/msCommonTypes.h"
#include "MidnightSunz/AbilitySystem/Data/msCharacterClassInfo.h"
#include "MidnightSunz/UI/WidgetController/msWidgetController_Overlay.h"
#include "ActiveGameplayEffectHandle.h"
#include "msCharacterBase.generated.h"

class UmsAbilitySystemComponent;
class UmsAttributeSet;
class UGameplayEffect;
class UGameplayAbility;
class UWidgetComponent;
class UAnimMontage;
class UNiagaraSystem;
class USonundBase;
class USkeletalMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDebuffSignature, const FGameplayTag&, DebuffTag, const int32, DebuffStack);

UCLASS()
class MIDNIGHTSUNZ_API AmsCharacterBase : public ACharacter, public IAbilitySystemInterface, public ImsCombatInterface
{
	GENERATED_BODY()

public:
	AmsCharacterBase();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UmsAttributeSet* GetAttributeSet() const;

	// begin ImsCombatInterface
	virtual USkeletalMeshComponent* GetWeapon_Implementation() const override;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const override;
	virtual UAnimMontage* GetHitReactMontate_Implementation() const override;
	virtual void Die() override;
	virtual FOnDamageSignature& GetOnDamageSignature() override;
	virtual FOnDeathSignature& GetOnDeathDelegate() override;
	virtual FOnBuffChangeSignature& GetOnBuffChangeDelegate() override;
	virtual bool IsFullHealth() override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation(EAttackType AttackType) const override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() const override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) const override;
	virtual int32 GetSummonedActorCount_Implementation() const override;
	virtual void AddSummonedActor_Implementation(AActor* InActor) override;
	virtual bool IsBeingElectrocuted_Implementation() const override;
	virtual void SetIsBeingElectrocuted_Implementation(bool bInElectrocute) override;
	virtual void GetBuffActiveEffectHandles(const FGameplayTag& InTag, TArray<FActiveGameplayEffectHandle>& OutHandles) const override;
	virtual float GetMaxHealth_Implementation() const override;
	// end ImsCombatInterface

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();

	ETurnInPlace GetTurnInPlace() const { return TurnInPlace; }
	virtual FVector GetCursorLocation() const { return FVector(); };
	virtual float GetAimPitch() const { return 0.f; };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;

	FOnDamageSignature OnDamageDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnDeathSignature OnDeathDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnBuffChangeSignature OnBuffChangeDelegate;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsBeingElectrocuted = false;

	// 버프 태그와 해당 버프의 활성화 GEHandle 맵핑.
	TMap<FGameplayTag, TArray<FActiveGameplayEffectHandle>> CachedBuffActiveEffectHandles;

	UPROPERTY(BlueprintAssignable)
	FOnDebuffSignature OnDebuffDeleaget;

	UPROPERTY(ReplicatedUsing = OnRep_PoisonStack)
	int32 PoisonStack;

	UFUNCTION()
	void OnRep_PoisonStack();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds);
	virtual void Destroyed() override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual float GetMovementSpeed() const;

	virtual void InitAbilityActorInfo();
	virtual void InitializeDefaultAttributes() const;
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;
	void AddCharacterAbilities();

	void Dissolve();

	void SetOverlayMaterial(UMaterialInstance* InOverlayMaterial);

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION()
	void OnDestroySummonedActor(AActor* DestroyedActor);

	virtual void DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	virtual void DebuffPoisonStackChanged(const FGameplayTag CallbackTag, int32 NewCount);
	void HandleDebuff(const FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHandleDebuff(const FGameplayTag& InDebuffTag);

	void HandleIceDebuff(bool bActivate);

	void BuffTagChanged(const FGameplayTag InBuffTag, int32 NewCount);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowBuffText(const FGameplayTag& BuffTag);

	void RegisterDebuffTagEvent();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Defualts")
	ECharacterType CharacterClass = ECharacterType::None;

	UPROPERTY(EditAnywhere)
	float DefaultWalkSpped = 350.f;

	UPROPERTY(Transient)
	TObjectPtr<UmsAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(Transient)
	TObjectPtr<UmsAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitaltAttributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnAttributeChangedSignature OnHealthChange;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnAttributeChangedSignature OnMaxHealthChange;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|MI")
	TObjectPtr<UMaterialInstance> DissolveMaterialnstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|MI")
	TMap<FGameplayTag, TObjectPtr<UMaterialInstance>> DebuffMaterialInstance;

	UPROPERTY(BlueprintReadOnly)
	ETurnInPlace TurnInPlace;

	bool bDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UNiagaraSystem* BloodEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	USoundBase* DeathSound;

	int32 SummonedEntitiesCount = 0;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> SummonedActors;

	TArray<FGameplayTag> AppliedDebuffs;

private:
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName ProjectileSpawnSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftHandSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightHandSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
};
