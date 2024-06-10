// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "msWidgetController.h"
#include "msWidgetController_Overlay.generated.h"

/**
 *
 */

struct FAbilityInfo;
struct FGameplayTag;
class UmsAbilityInfo;
class UmsAbilitySystemComponent;
class UmsIconInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAbilityInfo&, Info);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityTagSignature, const FGameplayTag&, Tag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateGameStageSignature, int32, IntParam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdatePlayerStateSignature, int32, IntParam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStageChangedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangedAbilityStatusSignature, const FGameplayTag&, AbilityTag, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangePointTagListSignature, const TArray<FGameplayTag>&, Tags);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FChangeArtifactSignature, const FGameplayTag&, ArfifactTag, int32, StackCount);

UCLASS(BlueprintType, Blueprintable)
class MIDNIGHTSUNZ_API UmsWidgetController_Overlay : public UmsWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UFUNCTION(BlueprintCallable)
	void PointButtonPressed();

	UFUNCTION(BlueprintCallable)
	void EvolutionSlotPressed(const FGameplayTag& EvolutionTag);

	UFUNCTION(BlueprintCallable)
	FGameplayTag GetAbilityTagFromInputTag(const FGameplayTag& InputTag);

	UFUNCTION(BlueprintCallable)
	int32 GetArtifactLevel(const FGameplayTag& InTag);
	UFUNCTION(BlueprintCallable)
	int32 GetAbilityLevel(const FGameplayTag& InTag);

	UFUNCTION(BlueprintCallable)
	void GetEvolutionText(const FGameplayTag& InTag, int32 InLevel, FText& OutTitle, FText& OutDesc);
	UFUNCTION(BlueprintCallable)
	void GetAbilityText(const FGameplayTag& InTag, int32 InLevel, FText& OutTitle, FText& OutDesc);

	UPROPERTY(BlueprintAssignable, Category = "GAS | Attribute")
	FOnAttributeChangedSignature OnHealthChange;

	UPROPERTY(BlueprintAssignable, Category = "GAS | Attribute")
	FOnAttributeChangedSignature OnMaxHealthChange;

	UPROPERTY(BlueprintAssignable, Category = "GAS | Ability")
	FAbilityInfoSignature AbilityInfoDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS | Ability")
	FAbilityTagSignature OnNewBuffDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Objective")
	FOnUpdateGameStageSignature OnUpdateObjectiveCountDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Objective")
	FOnUpdateGameStageSignature OnUpdateStageNumDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Stage State")
	FOnStageChangedSignature StartStartDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Stage State")
	FOnStageChangedSignature WaitStageDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Stage State")
	FOnStageChangedSignature GameOverDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Player State")
	FOnUpdatePlayerStateSignature ChangePointDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Player State")
	FChangePointTagListSignature ChangePointTagListDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Player State")
	FChangeArtifactSignature ChangeArtifactDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Player State")
	FOnChangedAbilityStatusSignature OnChangedAbilityStatusDelegate;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WidgetData")
	TObjectPtr<UmsAbilityInfo> AbilityInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WidgetData")
	TObjectPtr<UmsIconInfo> IconInfo;

	void OnInitializeStartupAbilities(UmsAbilitySystemComponent* ASC);

	UFUNCTION()
	void BindGameStateDelegate();

	UFUNCTION()
	void BuffTagChanged(const FGameplayTag& InBuffTag, int32 NewCount);
};
