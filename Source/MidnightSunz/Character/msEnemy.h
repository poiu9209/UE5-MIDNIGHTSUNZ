// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "msCharacterBase.h"
#include "msEnemy.generated.h"

class UBehaviorTree;
class AmsAIController;
class UAnimMontage;


/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API AmsEnemy : public AmsCharacterBase
{
	GENERATED_BODY()

public:
	AmsEnemy();
	virtual void PossessedBy(AController* NewController) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// begin ImsCombatInterface
	virtual int32 GetPlayerLevel_Implementation() override;
	virtual void BP_Die_Implementation() override;
	virtual void Die() override;
	virtual void SetIsBeingElectrocuted_Implementation(bool bInElectrocute) override;
	// end ImsCombatInterface


protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;

private:
	UFUNCTION()
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	void PlaySpanwMontage();

	UFUNCTION()
	void FinishSpawnMontage();

	UFUNCTION()
	void ShowActorVisible();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defualts")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Combat")
	bool bHitReact = false;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = "AI")
	bool bIsSpawnFinished;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> SpawnMontage;

	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AmsAIController> msAIController;

	UPROPERTY(EditAnywhere)
	bool bRangedAttacker = false;

private:
	FTimerHandle DieTimerHandle;

	FTimerHandle SpawnTimerHanlde;
	FTimerHandle SpawnHideTimerHanlde;
};
