// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "GameplayTagContainer.h"
#include "msPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UmsInputConfig;
class UmsAbilitySystemComponent;
class ULevelSequence;
class ALevelSequenceActor;


/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API AmsPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AmsPlayerController();

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(ACharacter* TargetCharacter, const FDamageTextParams& DamageTextParams);

	void OnStageStateSet(FName State);

	UFUNCTION(BlueprintCallable)
	bool IsDisableGameplay() const { return bDisableGameplay; }

	UFUNCTION(BlueprintCallable)
	void SetDiableGameplay(bool bInDisableGameplay) { bDisableGameplay = bInDisableGameplay; }

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:
	void Move(const FInputActionValue& InputActionValue);
	void InteractionStart(const FInputActionValue& InputActionValue);
	void InteractionComplete(const FInputActionValue& InputActionValue);

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	void HandleStageStartState();
	void HandleGameOverState();

	UFUNCTION()
	void OnRep_StageState();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractionAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UmsInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UmsAbilitySystemComponent> AbilitySystemComponent;

	UmsAbilitySystemComponent* GetAbilitySystemComponent();

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	bool bPreseedLMB = false;

	UPROPERTY(ReplicatedUsing = OnRep_StageState)
	FName StageState;

	UPROPERTY(Transient)
	bool bDisableGameplay = false;


};
