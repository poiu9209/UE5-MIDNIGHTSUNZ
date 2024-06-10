// Fill out your copyright notice in the Description page of Project Settings.


#include "msAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MidnightSunz/AbilitySystem/msAbilitySystemComponent.h"
#include "MidnightSunz/Interface/msCombatInterface.h"
#include "MidnightSunz/Player/msPlayerState.h"
#include "MidnightSunz/Player/msPlayerController.h"
#include "MidnightSunz/UI/HUD/msHUD.h"
#include "MidnightSunz/UI/WidgetController/msWidgetController.h"
#include "MidnightSunz/GameModes/msGameMode.h"
#include "MidnightSunz/Interface/msCombatInterface.h"
#include "MidnightSunz/msGameplayEffectContext.h"
#include "MidnightSunz/msGameplayTags.h"
#include "MidnightSunz/Actor/msProjectile.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"

bool UmsAbilitySystemLibrary::GetWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams, AmsHUD*& OutHUD)
{
	if (AmsPlayerController* PC = Cast<AmsPlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, 0)))
	{
		OutHUD = Cast<AmsHUD>(PC->GetHUD());
		if (OutHUD)
		{
			AmsPlayerState* PS = PC->GetPlayerState<AmsPlayerState>();
			if (!PS) return false;
			UmsAbilitySystemComponent* ASC = Cast<UmsAbilitySystemComponent>(PS->GetAbilitySystemComponent());
			UmsAttributeSet* AS = PS->GetAttributeSet();

			OutWCParams.AttributeSet = AS;
			OutWCParams.AbilitySystemComponent = ASC;
			OutWCParams.PlayerState = PS;
			OutWCParams.PlayerController = PC;
			return true;
		}
	}
	return false;
}

UmsWidgetController_Overlay* UmsAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AmsHUD* HUD = nullptr;
	if (GetWidgetControllerParams(WorldContextObject, WCParams, HUD))
	{
		return HUD->GetOverlayWidgetController(WCParams);
	}
	return nullptr;
}

UmsWidgetController_Attribute* UmsAbilitySystemLibrary::GetAttributeWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AmsHUD* HUD = nullptr;
	if (GetWidgetControllerParams(WorldContextObject, WCParams, HUD))
	{
		return HUD->GetAttributeMenuWidgetController(WCParams);
	}
	return nullptr;
}

UmsWidgetController_Buff* UmsAbilitySystemLibrary::GetBuffWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AmsHUD* HUD = nullptr;
	if (GetWidgetControllerParams(WorldContextObject, WCParams, HUD))
	{
		return HUD->GetBuffWidgetController(WCParams);
	}
	return nullptr;
}

void UmsAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterType CharacterType, float Level, UAbilitySystemComponent* ASC)
{
	const AActor* AvatarActor = ASC->GetAvatarActor();
	check(AvatarActor);

	UmsCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (!CharacterClassInfo)
	{
		return;
	}

	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterType);

	FGameplayEffectContextHandle EffectHandle = ASC->MakeEffectContext();
	EffectHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle AttributeSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.Attributes, Level, EffectHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*AttributeSpecHandle.Data.Get());
}

void UmsAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterType CharacterType)
{
	UmsCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (!CharacterClassInfo)
	{
		return;
	}

	for (const auto& AbilityClass : CharacterClassInfo->CommonAbilites)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}

	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterType);
	for (const auto& AbilityClass : DefaultInfo.StartupAbilites)
	{
		if (ImsCombatInterface* CI = Cast<ImsCombatInterface>(ASC->GetAvatarActor()))
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ImsCombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

UmsCharacterClassInfo* UmsAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	if (AmsGameMode* GM = Cast<AmsGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return GM->CharacterClassInfo;
	}

	return nullptr;
}

void UmsAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*> ActorsToIngore, float Radius, const FVector& ShpereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIngore);

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(Overlaps, ShpereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
		for (auto& Overlap : Overlaps)
		{
			if (Overlap.GetActor() && Overlap.GetActor()->Implements<UmsCombatInterface>() && !ImsCombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(Overlap.GetActor());
			}
		}
	}
}

bool UmsAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	if (!IsValid(FirstActor) || !IsValid(SecondActor))
	{
		return false;
	}

	const bool bBothPlayer = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothEnemy = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	const bool bFriends = bBothPlayer || bBothEnemy;
	return !bFriends;
}

FGameplayEffectContextHandle UmsAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	const FmsGameplayTags GameplayTags = FmsGameplayTags::Get();
	const AActor* SourceAvata = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();

	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvata);
	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(
		DamageEffectParams.DamageGameplayEffectClass,
		DamageEffectParams.AbilityLevel, EffectContextHandle);

	SetIsRadialDamage(EffectContextHandle, DamageEffectParams.bIsRadialDamage);
	SetRadialDamageInnerRadius(EffectContextHandle, DamageEffectParams.RadialDamageInnerRadius);
	SetRadialDamageOuterRadius(EffectContextHandle, DamageEffectParams.RadialDamageOuterRadius);
	SetRadialDamageOrigin(EffectContextHandle, DamageEffectParams.RadialDamageOrigin);
	SetIsArtifactDamage(EffectContextHandle, DamageEffectParams.bIsArtifactDamage);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Chance, DamageEffectParams.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Damage, DamageEffectParams.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Duration, DamageEffectParams.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Debuff_Frequency, DamageEffectParams.DebuffFrequency);

	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	return EffectContextHandle;
}

TArray<FRotator> UmsAbilitySystemLibrary::EvenlySpaceRotators(const FVector& Forward, const FVector& Axis, float Spread, int32 NumRotators)
{
	TArray<FRotator> Rotators;
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (NumRotators > 1)
	{
		const float DeltaSpread = Spread / (NumRotators - 1);
		for (int32 i = 0; i < NumRotators; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		Rotators.Add(Forward.Rotation());
	}

	return Rotators;
}

TArray<FVector> UmsAbilitySystemLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors)
{
	TArray<FVector> Vectors;
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	if (NumVectors > 1)
	{
		const float DeltaSpread = Spread / (NumVectors - 1);
		for (int32 i = 0; i < NumVectors; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Vectors.Add(Direction);
		}
	}
	else
	{
		Vectors.Add(Forward);
	}

	return Vectors;
}

FGameplayAbilitySpec* UmsAbilitySystemLibrary::GetSpecFromAbilityTag(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& AbilityTag)
{
	if (UmsAbilitySystemComponent* ASC = Cast<UmsAbilitySystemComponent>(AbilitySystemComponent))
	{
		return ASC->GetSpecFromAbilityTag(AbilityTag);
	}
	return nullptr;
}

FGameplayEffectContextHandle UmsAbilitySystemLibrary::ApplyDebuffDamage(const FDamageEffectParams& DamageEffectParams)
{
	const FmsGameplayTags GameplayTags = FmsGameplayTags::Get();
	const AActor* SourceAvata = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();

	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvata);
	FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, EffectContextHandle);

	SetIsRadialDamage(EffectContextHandle, DamageEffectParams.bIsRadialDamage);
	SetRadialDamageInnerRadius(EffectContextHandle, DamageEffectParams.RadialDamageInnerRadius);
	SetRadialDamageOuterRadius(EffectContextHandle, DamageEffectParams.RadialDamageOuterRadius);
	SetRadialDamageOrigin(EffectContextHandle, DamageEffectParams.RadialDamageOrigin);

	if (FmsGameplayEffectContext* msContext = static_cast<FmsGameplayEffectContext*>(SpecHandle.Data.Get()->GetContext().Get()))
	{
		TSharedPtr<FGameplayTag> DamageType = MakeShareable(new FGameplayTag(DamageEffectParams.DamageType));
		msContext->SetDamageType(DamageType);
	}

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);

	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	return EffectContextHandle;
}

bool UmsAbilitySystemLibrary::GetBuffTagFromGameplayEffect(const TSubclassOf<UGameplayEffect> InEffect, FGameplayTag& OutBuffTag)
{
	if (!InEffect)
	{
		return false;
	}

	for (const FGameplayTag& Tag : InEffect.GetDefaultObject()->InheritableOwnedTagsContainer.CombinedTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Buff"))))
		{
			OutBuffTag = Tag;
			return true;
		}
	}


	return false;
}

AActor* UmsAbilitySystemLibrary::SpawnActor(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, FTransform SpawnTransform, AActor* SpawnOwner)
{
	if (UWorld* World = WorldContextObject->GetWorld())
	{
		FActorSpawnParameters SpawnParam;
		SpawnParam.Owner = SpawnOwner;
		SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		return World->SpawnActor<AActor>(ActorClass, SpawnTransform, SpawnParam);
	}
	return nullptr;
}

AActor* UmsAbilitySystemLibrary::SpawnProjectile(const UObject* WorldContextObject, TSubclassOf<AmsProjectile> ProjectileClass, FTransform SpawnTransform, AActor* SpawnOwner, const FDamageEffectParams& DamageEffectParams)
{
	if (!WorldContextObject || !WorldContextObject->GetWorld() || !ProjectileClass || !SpawnOwner)
	{
		return nullptr;
	}
	AmsProjectile* Projectile = WorldContextObject->GetWorld()->SpawnActorDeferred<AmsProjectile>(ProjectileClass, SpawnTransform, SpawnOwner, Cast<APawn>(SpawnOwner), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	Projectile->DamageEffectParams = DamageEffectParams;

	Projectile->FinishSpawning(SpawnTransform);
	return Projectile;
}

AmsProjectile* UmsAbilitySystemLibrary::SpawnHomingProjectile(const UObject* WorldContextObject, TSubclassOf<AmsProjectile> ProjectileClass, FTransform SpawnTransform, AActor* SpawnOwner, AActor* HomingTarget, float HomingAccelerationMagnitude, const FDamageEffectParams& DamageEffectParams)
{
	if (!WorldContextObject || !WorldContextObject->GetWorld() || !ProjectileClass || !SpawnOwner || !HomingTarget)
	{
		return nullptr;
	}
	AmsProjectile* Projectile = WorldContextObject->GetWorld()->SpawnActorDeferred<AmsProjectile>(ProjectileClass, SpawnTransform, SpawnOwner, Cast<APawn>(SpawnOwner), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	Projectile->DamageEffectParams = DamageEffectParams;
	Projectile->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
	Projectile->ProjectileMovement->HomingAccelerationMagnitude = HomingAccelerationMagnitude;
	Projectile->ProjectileMovement->bIsHomingProjectile = true;

	Projectile->FinishSpawning(SpawnTransform);
	return Projectile;
}

bool UmsAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FmsGameplayEffectContext* msEffectContext = static_cast<const FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return msEffectContext->IsCriticalHit();
	}
	return false;
}

bool UmsAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FmsGameplayEffectContext* msEffectContext = static_cast<const FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return msEffectContext->IsSuccessfulDebuff();
	}
	return false;
}

float UmsAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FmsGameplayEffectContext* msEffectContext = static_cast<const FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return msEffectContext->GetDebuffDamage();
	}
	return 0.0f;
}

float UmsAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FmsGameplayEffectContext* msEffectContext = static_cast<const FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return msEffectContext->GetDebuffDuration();
	}
	return 0.0f;
}

float UmsAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FmsGameplayEffectContext* msEffectContext = static_cast<const FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return msEffectContext->GetDebuffFrequency();
	}
	return 0.0f;
}

FGameplayTag UmsAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FmsGameplayEffectContext* msEffectContext = static_cast<const FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (msEffectContext->GetDamageType().IsValid())
		{
			return *msEffectContext->GetDamageType();
		}
	}
	return FGameplayTag();
}

bool UmsAbilitySystemLibrary::IsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FmsGameplayEffectContext* msEffectContext = static_cast<const FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return msEffectContext->IsRadialDamage();
	}
	return false;
}

bool UmsAbilitySystemLibrary::IsArtifactDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FmsGameplayEffectContext* msEffectContext = static_cast<const FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return msEffectContext->IsArtifactDamage();
	}
	return false;
}

float UmsAbilitySystemLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FmsGameplayEffectContext* msEffectContext = static_cast<const FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return msEffectContext->GetRadialDamageInnerRadius();
	}
	return 0.0f;
}

float UmsAbilitySystemLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FmsGameplayEffectContext* msEffectContext = static_cast<const FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return msEffectContext->GetRadialDamageOuterRadius();
	}
	return 0.0f;
}

FVector UmsAbilitySystemLibrary::GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FmsGameplayEffectContext* msEffectContext = static_cast<const FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return msEffectContext->GetRadialDamageOrigin();
	}

	return FVector::ZeroVector;
}

bool UmsAbilitySystemLibrary::IsBlockedDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FmsGameplayEffectContext* msEffectContext = static_cast<const FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return msEffectContext->IsBlockedDmage();
	}
	return false;
}

void UmsAbilitySystemLibrary::SetIsCriticalHit(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (FmsGameplayEffectContext* msEffectContext = static_cast<FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		msEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UmsAbilitySystemLibrary::SetIsSuccessfulDebuff(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool bInIsSuccessfulDebuff)
{
	if (FmsGameplayEffectContext* msEffectContext = static_cast<FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		msEffectContext->SetIsSuccessfulDebuff(bInIsSuccessfulDebuff);
	}
}

void UmsAbilitySystemLibrary::SetDebuffDamage(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, float InDamage)
{
	if (FmsGameplayEffectContext* msEffectContext = static_cast<FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		msEffectContext->SetDebuffDamage(InDamage);
	}
}

void UmsAbilitySystemLibrary::SetDebuffDuration(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, float InDuration)
{
	if (FmsGameplayEffectContext* msEffectContext = static_cast<FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		msEffectContext->SetDebuffDuration(InDuration);
	}
}

void UmsAbilitySystemLibrary::SetDebuffFrequency(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, float InFrequency)
{
	if (FmsGameplayEffectContext* msEffectContext = static_cast<FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		msEffectContext->SetDebuffFrequency(InFrequency);
	}
}

void UmsAbilitySystemLibrary::SetDamageType(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType)
{
	if (FmsGameplayEffectContext* msEffectContext = static_cast<FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		msEffectContext->SetDamageType(DamageType);
	}
}

void UmsAbilitySystemLibrary::SetIsRadialDamage(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool bInIsRadialDamage)
{
	if (FmsGameplayEffectContext* msEffectContext = static_cast<FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		msEffectContext->SetIsRadialDamage(bInIsRadialDamage);
	}
}

void UmsAbilitySystemLibrary::SetIsArtifactDamage(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool bIsIsArtifactDamage)
{
	if (FmsGameplayEffectContext* msEffectContext = static_cast<FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		msEffectContext->SetIsArtifactDamage(bIsIsArtifactDamage);
	}
}

void UmsAbilitySystemLibrary::SetRadialDamageInnerRadius(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, float InInnerRadius)
{
	if (FmsGameplayEffectContext* msEffectContext = static_cast<FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		msEffectContext->SetRadialDamageInnerRadius(InInnerRadius);
	}
}

void UmsAbilitySystemLibrary::SetRadialDamageOuterRadius(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, float InOuterRadius)
{
	if (FmsGameplayEffectContext* msEffectContext = static_cast<FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		msEffectContext->SetRadialDamageOuterRadius(InOuterRadius);
	}
}

void UmsAbilitySystemLibrary::SetRadialDamageOrigin(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, const FVector& InOrigin)
{
	if (FmsGameplayEffectContext* msEffectContext = static_cast<FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		msEffectContext->SetRadialDamageOrigin(InOrigin);
	}
}

void UmsAbilitySystemLibrary::SetIsBlockedDamage(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, const bool InBlocked)
{
	if (FmsGameplayEffectContext* msEffectContext = static_cast<FmsGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		msEffectContext->SetIsBlockedDamage(InBlocked);
	}
}
