// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "MidnightSunz/GameModes/msGameTypes.h"
#include "msPlayerState.generated.h"

class UAbilitySystemComponent;
class UmsAbilitySystemComponent;
class UmsAttributeSet;
class UmsArtifact;
class UmsArtifact_KillEnemy;
class UmsArtifact_HitEnemy;
class UmsArtifact_NormalAttack;
struct FGameplayTag;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdatePointSignature, int32 /*Points*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdatePointTagListSignature, const TArray<FGameplayTag>& /*Point Tags*/)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUpdateArtifactSignature, const FGameplayTag& /*Point Tags*/, int32 /*Artifact Stack*/)
DECLARE_MULTICAST_DELEGATE(FOnInitGameState)


/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API AmsPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AmsPlayerState();
	virtual void BeginPlay();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UmsAttributeSet* GetAttributeSet() const;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	FORCEINLINE int32 GetPlayerLevel() const { return Level; }

	void GetSaveData(FPlayerSaveData& OutData);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDiableGameplay();

	//test
	void SetLevel(int32 InLV) { Level = InLV; }

	void AddEvolutionOpion(const FEvolutionOption& InOption);

	FOnUpdatePointSignature OnUpdatePointDelegate;
	FOnUpdatePointTagListSignature OnUpdatePointTagListDelegate;
	FOnUpdateArtifactSignature OnUpdateArtifactDelegate;
	FOnInitGameState OnInitGameState;

	int32 GetEvolutionPointNum() const { return EvolutionOptions.Num(); }
	int32 GetArtifactStack(const FGameplayTag& InTag) const;

	UFUNCTION(Client, Reliable)
	void ClientAckPointTagList(const TArray<FGameplayTag>& TagList);

	UFUNCTION(Server, Reliable)
	void ServerRequestEvolution(const FGameplayTag& EvolutionTag);

	void RequestEvolutionTags();

	void ArtifactUpgrade(const FGameplayTag& InTag);
	void NewArtifact(const FGameplayTag& InTag, int32 InStack);
	void AddArtifactStack(const FGameplayTag& InTag);
	void ApplySavedArtifactAndEvolution(FPlayerSaveData& Data);

	void TriggerKillEnemyArtifacts(const FVector& TargetLocation);
	void TriggerHitEnemyArtifacts(AActor* HitActor);
	void TriggerNormalAttackArtifact();
	void StoreBaseDamage();
	float GetArtifactProbability(const FGameplayTag& InArtifactTag) const;
	bool CheckArtifactProbability(const FGameplayTag& InArtifactTag) const;
	float GetBaseDamage() const { return BaseDamage; }

	void InitGameState();
	void CheatAddSkillLevel();
protected:
	UFUNCTION()
	void ArtifactChanged(const FGameplayTag CallbackTag, int32 NewCount);

	void RegisterArtifactEvent();
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UmsAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UmsAttributeSet> AttributeSet;

	UPROPERTY()
	TMap<FGameplayTag, uint8> ArtifactInfoMap;	// Value = Stack

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UmsArtifact>> ArtifactMap;

	UPROPERTY()
	TArray<TObjectPtr<UmsArtifact_KillEnemy>> KillEnemyArtifacts;

	UPROPERTY()
	TArray<TObjectPtr<UmsArtifact_HitEnemy>> HitEnemyArtifacts;

	UPROPERTY()
	TArray<TObjectPtr<UmsArtifact_NormalAttack>> NormalAttackArtifacts;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 Level = 1.f;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_EvolutionOptions)
	TArray<FEvolutionOption> EvolutionOptions;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UFUNCTION()
	void OnRep_EvolutionOptions();

	// 좌클릭 평타 데미지
	float BaseDamage;

	UPROPERTY(Transient)
	bool bRegisterArtifactEvent = false;
};
