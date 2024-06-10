// Fill out your copyright notice in the Description page of Project Settings.


#include "msProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "MidnightSunz/MidnightSunz.h"
#include "MidnightSunz/Character/msCharacter.h"
#include "MidnightSunz/AbilitySystem/msAbilitySystemLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayCueManager.h"


AmsProjectile::AmsProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSimulationIterations = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AmsProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
	SetReplicateMovement(true);

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AmsProjectile::Destroyed()
{
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}

	if (!bHit && !HasAuthority())
	{
		OnHit();
	}

	Super::Destroyed();
}

void AmsProjectile::OnHit()
{
	ExecuteImpactCue();

	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
	}
	bHit = true;
}

bool AmsProjectile::IsValidOverlap(AActor* OtherActor)
{
	if (!DamageEffectParams.SourceAbilitySystemComponent)
	{
		return false;
	}

	AActor* SourceAvataActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	if (SourceAvataActor == OtherActor)
	{
		return false;
	}

	if (!IsValid(SourceAvataActor) || !IsValid(OtherActor))
	{
		return false;
	}

	if (!UmsAbilitySystemLibrary::IsNotFriend(SourceAvataActor, OtherActor))
	{
		return false;
	}

	return true;
}

void AmsProjectile::ExecuteImpactCue()
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();

		ASC->ExecuteGameplayCue(ImpactCueTag, CueParams);
	}
}

void AmsProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor))
	{
		return;
	}

	if (!bHit)
	{
		OnHit();
	}

	if (HasAuthority())
	{
		DamageEffectParams.TargetAbilitySystemComponent = DamageEffectParams.TargetAbilitySystemComponent ? DamageEffectParams.TargetAbilitySystemComponent : UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
		if (DamageEffectParams.TargetAbilitySystemComponent)
		{
			UmsAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
		Destroy();
	}
	else
	{
		bHit = true;
	}
}
