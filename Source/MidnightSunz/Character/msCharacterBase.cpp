// Fill out your copyright notice in the Description page of Project Settings.


#include "msCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "MidnightSunz/MidnightSunz.h"
#include "MidnightSunz/AbilitySystem/msAbilitySystemComponent.h"
#include "MidnightSunz/AbilitySystem/Attributes/msAttributeSet.h"
#include "MidnightSunz/msGameplayTags.h"
#include "MidnightSunz/msLogChannels.h"
#include "MidnightSunz/GameModes/msGameMode.h"

AmsCharacterBase::AmsCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);

	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), FName("Weapon_RSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AmsCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AmsCharacterBase, bIsBeingElectrocuted);
	DOREPLIFETIME(AmsCharacterBase, PoisonStack);
}

UAbilitySystemComponent* AmsCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UmsAttributeSet* AmsCharacterBase::GetAttributeSet() const
{
	return AttributeSet;
}

USkeletalMeshComponent* AmsCharacterBase::GetWeapon_Implementation() const
{
	return Weapon;
}

FVector AmsCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const
{
	const FmsGameplayTags& GameplayTags = FmsGameplayTags::Get();
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon) && IsValid(Weapon))
	{
		return Weapon->GetSocketLocation(ProjectileSpawnSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}

	return FVector();
}

UAnimMontage* AmsCharacterBase::GetHitReactMontate_Implementation() const
{
	return HitReactMontage;
}

void AmsCharacterBase::Die()
{
	if (auto GM = Cast<AmsGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->HandleCharacterDeath(this);
	}

	MulticastHandleDeath();
}

FOnDamageSignature& AmsCharacterBase::GetOnDamageSignature()
{
	return OnDamageDelegate;
}

FOnDeathSignature& AmsCharacterBase::GetOnDeathDelegate()
{
	return OnDeathDelegate;
}

FOnBuffChangeSignature& AmsCharacterBase::GetOnBuffChangeDelegate()
{
	return OnBuffChangeDelegate;
}

bool AmsCharacterBase::IsFullHealth()
{
	if (AttributeSet)
	{
		return AttributeSet->GetHealth() >= AttributeSet->GetMaxHealth();
	}
	return false;
}

bool AmsCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* AmsCharacterBase::GetAvatar_Implementation()
{
	return this;
}

TArray<FTaggedMontage> AmsCharacterBase::GetAttackMontages_Implementation(EAttackType AttackType) const
{
	TArray<FTaggedMontage> Montages;
	for (const auto& Montage : AttackMontages)
	{
		if (Montage.AttackType == AttackType)
		{
			Montages.Add(Montage);
		}
	}
	return Montages;
}

void AmsCharacterBase::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AmsCharacterBase::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

UNiagaraSystem* AmsCharacterBase::GetBloodEffect_Implementation() const
{
	return BloodEffect;
}

FTaggedMontage AmsCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) const
{
	for (const auto& TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageTag.MatchesTagExact(MontageTag))
		{
			return TaggedMontage;
		}
	}
	return FTaggedMontage();
}

int32 AmsCharacterBase::GetSummonedActorCount_Implementation() const
{
	return SummonedActors.Num();
}

void AmsCharacterBase::AddSummonedActor_Implementation(AActor* InActor)
{
	if (!IsValid(InActor))
	{
		return;
	}

	InActor->Tags.Add(SUMMON_TAG);
	ImsCombatInterface* CombatInterface = CastChecked<ImsCombatInterface>(InActor);
	CombatInterface->GetOnDeathDelegate().AddDynamic(this, &ThisClass::OnDestroySummonedActor);
	SummonedActors.AddUnique(InActor);
}

bool AmsCharacterBase::IsBeingElectrocuted_Implementation() const
{
	return bIsBeingElectrocuted;
}

void AmsCharacterBase::SetIsBeingElectrocuted_Implementation(bool bInElectrocute)
{
	bIsBeingElectrocuted = bInElectrocute;

	if (bIsBeingElectrocuted)
	{
		GetCharacterMovement()->DisableMovement();
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}

void AmsCharacterBase::GetBuffActiveEffectHandles(const FGameplayTag& InTag, TArray<FActiveGameplayEffectHandle>& OutHandles) const
{
	if (CachedBuffActiveEffectHandles.Contains(InTag))
	{
		OutHandles.Append(CachedBuffActiveEffectHandles[InTag]);
	}
}

float AmsCharacterBase::GetMaxHealth_Implementation() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetMaxHealth();
	}

	return 0.0f;
}

void AmsCharacterBase::OnDestroySummonedActor(AActor* DestroyedActor)
{
	SummonedActors.RemoveAll([DestroyedActor](const TWeakObjectPtr<AActor>& Actor)
		{
			return !Actor.IsValid() || Actor == DestroyedActor;
		});
}

void AmsCharacterBase::RegisterDebuffTagEvent()
{
	// 화염, 냉기, 전기 
	AbilitySystemComponent->RegisterGameplayTagEvent(
		FmsGameplayTags::Get().Debuff_Fire, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::DebuffTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(
		FmsGameplayTags::Get().Debuff_Ice, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::DebuffTagChanged);
	AbilitySystemComponent->RegisterGameplayTagEvent(
		FmsGameplayTags::Get().Debuff_Lightning, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::DebuffTagChanged);
	// 독은 중첩효과라 이벤트 타입다르게 설정.
	AbilitySystemComponent->RegisterGameplayTagEvent(
		FmsGameplayTags::Get().Debuff_Poison, EGameplayTagEventType::AnyCountChange).AddUObject(this, &ThisClass::DebuffPoisonStackChanged);
}

void AmsCharacterBase::DebuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	HandleDebuff(CallbackTag, NewCount);

	OnDebuffDeleaget.Broadcast(CallbackTag, NewCount);
}

void AmsCharacterBase::DebuffPoisonStackChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	HandleDebuff(CallbackTag, NewCount);

	if (NewCount <= 0)
	{
		PoisonStack = 0;
		OnDebuffDeleaget.Broadcast(CallbackTag, NewCount);
		return;
	}

	FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CallbackTag.GetSingleTagContainer());
	TArray<FActiveGameplayEffectHandle> PoisonActiveEffectHandles = AbilitySystemComponent->GetActiveEffects(GameplayEffectQuery);
	if (PoisonActiveEffectHandles.Num() > 0)
	{
		int32 PrevStack = PoisonStack;
		int32 Stack = 0;
		for (const auto& EffectHandls : PoisonActiveEffectHandles)
		{
			int32 ccStack = AbilitySystemComponent->GetCurrentStackCount(EffectHandls);
			Stack = FMath::Max(Stack, ccStack);
		}
		PoisonStack = Stack;
		UE_LOG(Logms, Log, TEXT("[%s] Poison [%d] Update, [%s]"), *CallbackTag.GetTagName().ToString(), Stack, *GetNameSafe(this));

		if (PrevStack != PoisonStack)
		{
			OnDebuffDeleaget.Broadcast(CallbackTag, PoisonStack);
		}
	}
}

void AmsCharacterBase::HandleDebuff(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (bDead)
	{
		return;
	}

	const bool bAdd = NewCount > 0;
	if (bAdd)
	{
		if (AppliedDebuffs.Num() <= 0 || !AppliedDebuffs.Contains(CallbackTag))
		{
			MulticastHandleDebuff(CallbackTag);
			AppliedDebuffs.AddUnique(CallbackTag);
		}
	}
	else
	{
		AppliedDebuffs.Remove(CallbackTag);
		const FGameplayTag Tag = AppliedDebuffs.Num() > 0 ? AppliedDebuffs[AppliedDebuffs.Num() - 1] : FGameplayTag::EmptyTag;
		MulticastHandleDebuff(Tag);
	}
}

void AmsCharacterBase::MulticastHandleDebuff_Implementation(const FGameplayTag& InDebuffTag)
{
	if (InDebuffTag == FGameplayTag::EmptyTag)
	{
		SetOverlayMaterial(nullptr);
		HandleIceDebuff(false);
		return;
	}

	if (DebuffMaterialInstance.Contains(InDebuffTag))
	{
		SetOverlayMaterial(DebuffMaterialInstance[InDebuffTag]);
	}

	if (InDebuffTag.MatchesTagExact(FmsGameplayTags::Get().Debuff_Ice))
	{
		HandleIceDebuff(true);
	}
}

void AmsCharacterBase::HandleIceDebuff(bool bActivate)
{
	const float Speed = GetMovementSpeed();
	if (bActivate)
	{
		GetCharacterMovement()->MaxWalkSpeed = Speed * 0.5f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = Speed;
	}
}

void AmsCharacterBase::BuffTagChanged(const FGameplayTag InBuffTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		UE_LOG(Logms, Log, TEXT("[%s] Buff Removed, [%s]"), *InBuffTag.GetTagName().ToString(), *GetNameSafe(this));

		CachedBuffActiveEffectHandles.Remove(InBuffTag);
		return;
	}

	// 버프 태그와 연관된 ActiveGameplayEffectHandle을 가져와서 캐싱
	FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InBuffTag.GetSingleTagContainer());
	TArray<FActiveGameplayEffectHandle> BuffActiveEffectHandles = AbilitySystemComponent->GetActiveEffects(GameplayEffectQuery);
	if (BuffActiveEffectHandles.Num() > 0)
	{
		UE_LOG(Logms, Log, TEXT("[%s] Buff Update, [%s]"), *InBuffTag.GetTagName().ToString(), *GetNameSafe(this));

		CachedBuffActiveEffectHandles.Add(InBuffTag, BuffActiveEffectHandles);
		OnBuffChangeDelegate.Broadcast(InBuffTag, NewCount);
		ShowBuffText(InBuffTag);
	}
}

void AmsCharacterBase::MulticastHandleDeath_Implementation()
{
	SetOverlayMaterial(nullptr);

	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
	GetMesh()->GetAnimInstance()->Montage_Stop(0.f);
	if (DeathMontage)
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		GetMesh()->GetAnimInstance()->Montage_Play(DeathMontage);
	}
	else
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetEnableGravity(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	}

	Dissolve();
	bDead = true;
	OnDeathDelegate.Broadcast(this);
}

void AmsCharacterBase::OnRep_PoisonStack()
{
	OnDebuffDeleaget.Broadcast(FmsGameplayTags::Get().Debuff_Poison, PoisonStack);
}

void AmsCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	bDead = false;
}

void AmsCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AmsCharacterBase::Destroyed()
{
	for (TWeakObjectPtr<AActor>& SummonedActor : SummonedActors)
	{
		if (AActor* Summon = SummonedActor.Get())
		{
			Summon->OnDestroyed.RemoveDynamic(this, &ThisClass::OnDestroySummonedActor);
		}
	}

	Super::Destroy();
}

float AmsCharacterBase::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageTaken = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	OnDamageDelegate.Broadcast(DamageTaken);
	return DamageTaken;
}

float AmsCharacterBase::GetMovementSpeed() const
{
	return DefaultWalkSpped;
}

void AmsCharacterBase::InitAbilityActorInfo()
{
	// Player -> PlayerStaet
	// Enemy -> Enemy
	// 따로 Init 해준다.
}

void AmsCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultVitaltAttributes, 1.f);
}

void AmsCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);

	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AmsCharacterBase::AddCharacterAbilities()
{
	if (!HasAuthority())
	{
		return;
	}

	if (AbilitySystemComponent->GetActivatableAbilities().Num() <= 0)
	{
		AbilitySystemComponent->AddCharacterAbilities(StartupAbilities);
	}
}

void AmsCharacterBase::Dissolve()
{
	if (IsValid(DissolveMaterialnstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialnstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);
		TArray<USceneComponent*> Child = GetMesh()->GetAttachChildren();
		for (auto Comp : Child)
		{
			if (USkeletalMeshComponent* SK = Cast<USkeletalMeshComponent>(Comp))
			{
				SK->SetMaterial(0, DynamicMatInst);
			}
		}

		StartDissolveTimeline(DynamicMatInst);
	}
}

void AmsCharacterBase::SetOverlayMaterial(UMaterialInstance* InOverlayMaterial)
{
	GetMesh()->SetOverlayMaterial(InOverlayMaterial);
	TArray<USceneComponent*> Child = GetMesh()->GetAttachChildren();
	for (auto Comp : Child)
	{
		if (USkeletalMeshComponent* SK = Cast<USkeletalMeshComponent>(Comp))
		{
			SK->SetOverlayMaterial(InOverlayMaterial);
		}
	}
}

