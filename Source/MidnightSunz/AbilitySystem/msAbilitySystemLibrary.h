// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/msCharacterClassInfo.h"
#include "GameplayAbilitySpec.h"
#include "msAbilitySystemLibrary.generated.h"

class UmsWidgetController_Attribute;
class UmsWidgetController_Overlay;
class UmsCharacterClassInfo;
class UAbilitySystemComponent;
class UmsWidgetController_Buff;
class AmsProjectile;
/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API UmsAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "msAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static bool GetWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AmsHUD*& OutHUD);

	UFUNCTION(BlueprintPure, Category = "msAbilitySystemLibrary|WidgetController")
	static UmsWidgetController_Overlay* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "msAbilitySystemLibrary|WidgetController")
	static UmsWidgetController_Attribute* GetAttributeWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "msAbilitySystemLibrary|WidgetController")
	static UmsWidgetController_Buff* GetBuffWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterType CharacterType, float Level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterType CharacterType);

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary|CharacterClassDefaults")
	static UmsCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*> ActorsToIngore, float Radius, const FVector& ShpereOrigin);

	UFUNCTION(BlueprintPure, Category = "msAbilitySystemLibrary")
	static bool IsNotFriend(AActor* FirstActor, AActor* SecondActor);

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary")
	static FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams);

	UFUNCTION(BlueprintPure, Category = "msAbilitySystemLibrary")
	static TArray<FRotator> EvenlySpaceRotators(const FVector& Forward, const FVector& Axis, float Spread, int32 NumRotators);

	UFUNCTION(BlueprintPure, Category = "msAbilitySystemLibrary")
	static TArray<FVector> EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors);

	static FGameplayAbilitySpec* GetSpecFromAbilityTag(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& AbilityTag);

	static FGameplayEffectContextHandle ApplyDebuffDamage(const FDamageEffectParams& DamageEffectParams);

	static bool GetBuffTagFromGameplayEffect(const TSubclassOf<UGameplayEffect> InEffect, FGameplayTag& OutBuffTag);

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary")
	static AActor* SpawnActor(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, FTransform SpawnTransform, AActor* SpawnOwner);

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary")
	static AActor* SpawnProjectile(const UObject* WorldContextObject, TSubclassOf<AmsProjectile> ProjectileClass, FTransform SpawnTransform, AActor* SpawnOwner, const FDamageEffectParams& DamageEffectParams);

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary")
	static AmsProjectile* SpawnHomingProjectile(const UObject* WorldContextObject, TSubclassOf<AmsProjectile> ProjectileClass, FTransform SpawnTransform, AActor* SpawnOwner, AActor* HomingTarget, float HomingAccelerationMagnitude, const FDamageEffectParams& DamageEffectParams);

	UFUNCTION(BlueprintPure, Category = "msAbilitySystemLibrary||GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "msAbilitySystemLibrary||GameplayEffects")
	static bool IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "msAbilitySystemLibrary||GameplayEffects")
	static float GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "msAbilitySystemLibrary||GameplayEffects")
	static float GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "msAbilitySystemLibrary||GameplayEffects")
	static float GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "msAbilitySystemLibrary||GameplayEffects")
	static FGameplayTag GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle);

	static bool IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	static bool IsArtifactDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "msAbilitySystemLibrary|GameplayEffects")
	static float GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "msAbilitySystemLibrary|GameplayEffects")
	static float GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "msAbilitySystemLibrary|GameplayEffects")
	static FVector GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle);

	static bool IsBlockedDamage(const FGameplayEffectContextHandle& EffectContextHandle);

	/*
	* Effect Context Setters
	*/

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary||GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary||GameplayEffects")
	static void SetIsSuccessfulDebuff(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsSuccessfulDebuff);

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary||GameplayEffects")
	static void SetDebuffDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InDamage);

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary||GameplayEffects")
	static void SetDebuffDuration(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InDuration);

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary||GameplayEffects")
	static void SetDebuffFrequency(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InFrequency);

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary||GameplayEffects")
	static void SetDamageType(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType);

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary|GameplayEffects")
	static void SetIsRadialDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsRadialDamage);

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary|GameplayEffects")
	static void SetIsArtifactDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bIsIsArtifactDamage);

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageInnerRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InInnerRadius);

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageOuterRadius(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, float InOuterRadius);

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary|GameplayEffects")
	static void SetRadialDamageOrigin(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FVector& InOrigin);

	UFUNCTION(BlueprintCallable, Category = "msAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockedDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const bool InBlocked);
};
