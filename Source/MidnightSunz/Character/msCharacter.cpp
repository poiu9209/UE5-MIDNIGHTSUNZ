// Fill out your copyright notice in the Description page of Project Settings.


#include "msCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MidnightSunz/GameModes/msGameMode.h"
#include "MidnightSunz/GameModes/msGameInstance.h"
#include "MidnightSunz/AbilitySystem/msAbilitySystemComponent.h"
#include "MidnightSunz/AbilitySystem/Attributes/msAttributeSet.h"
#include "MidnightSunz/Player/msPlayerState.h"
#include "MidnightSunz/Player/msPlayerController.h"
#include "MidnightSunz/UI/HUD/msHUD.h"
#include "MidnightSunz/msGameplayTags.h"

AmsCharacter::AmsCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bInheritYaw = false;
	CameraBoom->TargetArmLength = 900.f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
}

void AmsCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilityActorInfo();
	AddCharacterAbilities();
	ApplySaveData();

	GetMyPS()->StoreBaseDamage();

	PlayerController = PlayerController ? PlayerController : Cast<AmsPlayerController>(GetController());
	PlayerController->SetDiableGameplay(false);
}

void AmsCharacter::ApplySaveData()
{
	if (UmsGameInstance* GI = GetGameInstance<UmsGameInstance>())
	{
		FPlayerSaveData Data;
		if (GI->GetSaveData(GetMyPS()->GetPlayerName(), Data))
		{
			AbilitySystemComponent->ApplySavedAbilityLevel(Data);
			GetMyPS()->ApplySavedArtifactAndEvolution(Data);
			GI->ClearData(GetMyPS()->GetPlayerName());
		}
	}
}

void AmsCharacter::OnRep_PlayerState()
{
	InitAbilityActorInfo();
}

void AmsCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	PlayerController = PlayerController ? PlayerController : Cast<AmsPlayerController>(GetController());
	PlayerController->SetDiableGameplay(false);
}

float AmsCharacter::GetMovementSpeed() const
{
	const float MovementIncrease = AttributeSet->GetMovementSpeedIncrease();
	return DefaultWalkSpped + (DefaultWalkSpped * MovementIncrease);
}

void AmsCharacter::Die()
{
	Super::Die();

	if (HasAuthority() && DeadAlretClass)
	{
		GetMyPS()->SetLevel(100);
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = this;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		DeadAlret = GetWorld()->SpawnActor<AActor>(DeadAlretClass, GetActorLocation(), GetActorRotation(), SpawnParams);

		if (IsLocallyControlled())
		{
			PlayerController = PlayerController ? PlayerController : Cast<AmsPlayerController>(GetController());
			PlayerController->SetDiableGameplay(true);

		}
		else
		{
			ClientDie();
		}
	}
}

void AmsCharacter::ClientDie_Implementation()
{
	if (IsLocallyControlled())
	{
		PlayerController = PlayerController ? PlayerController : Cast<AmsPlayerController>(GetController());
		PlayerController->SetDiableGameplay(true);
	}
}

int32 AmsCharacter::GetPlayerLevel_Implementation()
{
	return GetMyPS()->GetPlayerLevel();
}

void AmsCharacter::TriggerKillEnemyArtifact_Implementation(const FVector& TargetLocation)
{
	GetMyPS()->TriggerKillEnemyArtifacts(TargetLocation);
}

void AmsCharacter::TriggerHitEnemyArtifact_Implementation(AActor* TargetActor)
{
	GetMyPS()->TriggerHitEnemyArtifacts(TargetActor);
}

void AmsCharacter::TriggerNormalAttackArtifact_Implementation()
{
	GetMyPS()->TriggerNormalAttackArtifact();
}

float AmsCharacter::GetBaseDamage_Implementation()
{
	return GetMyPS()->GetBaseDamage();
}

float AmsCharacter::GetProbability_Implementation(const FGameplayTag& InArtifactTag)
{
	return GetMyPS()->GetArtifactProbability(InArtifactTag);
}

bool AmsCharacter::CheckProbability_Implementation(const FGameplayTag& InArtifactTag)
{
	return GetMyPS()->CheckArtifactProbability(InArtifactTag);
}

void AmsCharacter::Revive_Implementation()
{
	MulticastHandleRevive();
}

void AmsCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PlayerController = PlayerController ? PlayerController : Cast<AmsPlayerController>(GetController());
	if (PlayerController && PlayerController->IsDisableGameplay())
	{
		return;
	}
	TraceUnderCursor(DeltaSeconds);
	SetTunrinPlace();
}

void AmsCharacter::SetTunrinPlace()
{
	const float Speed = GetVelocity().Size();
	if (Speed > 0.f)
	{
		TurnInPlace = ETurnInPlace::NotTurning;
	}
	else
	{
		const float CurrentYaw = GetActorRotation().Yaw;
		const float DiffYaw = CurrentYaw - CharYaw;
		if (DiffYaw > 0.5f)
		{
			TurnInPlace = ETurnInPlace::TurnRight;
		}
		else if (DiffYaw < -0.5f)
		{
			TurnInPlace = ETurnInPlace::TunrnLeft;
		}
		else
		{
			TurnInPlace = ETurnInPlace::NotTurning;
		}

		CharYaw = CurrentYaw;
	}
}

void AmsCharacter::TraceUnderCursor(float DeltaTime)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if (AbilitySystemComponent && (AbilitySystemComponent->HasMatchingGameplayTag(FmsGameplayTags::Get().Player_Block_TraceCurosr) || AbilitySystemComponent->HasMatchingGameplayTag(FmsGameplayTags::Get().Debuff_Lightning)))
	{
		return;
	}

	PlayerController = PlayerController ? PlayerController : Cast<AmsPlayerController>(GetController());
	if (PlayerController)
	{
		FHitResult CursorHit;
		PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);

		const FVector HitLocation = CursorHit.Location;
		// Pawn 이 아닌경우 Z값 보정
		float CursorLocation_Z = HitLocation.Z;
		AActor* TargetActor = CursorHit.GetActor();
		if (!TargetActor)
		{
			CursorLocation_Z = BaseEyeHeight;
		}
		else
		{
			if (TargetActor->IsA<APawn>())
			{
				CursorLocation_Z = CursorLocation_Z;
			}
			else
			{
				CursorLocation_Z = BaseEyeHeight;

			}
		}

		CursorLocation = FVector(HitLocation.X, HitLocation.Y, CursorLocation_Z);

		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CursorLocation);
		CurrentLookAtRotation = UKismetMathLibrary::RInterpTo(CurrentLookAtRotation, FRotator(0.f, LookAtRot.Yaw + RotOffset, 0.f), DeltaTime, 20.f);
		SetActorRotation(FRotator(0.f, CurrentLookAtRotation.Yaw, 0.f));

		float LookAtRotPitch = 0.f;// FMath::Max<float>(0.f, LookAtRot.Pitch);
		AimPitch = UKismetMathLibrary::FInterpTo(AimPitch, LookAtRotPitch, DeltaTime, 300.f);
		ServerUpdateCursor(CurrentLookAtRotation.Yaw, AimPitch);
	}
}

AmsPlayerState* AmsCharacter::GetMyPS()
{
	if (!MyPS)
	{
		MyPS = GetPlayerState<AmsPlayerState>();
	}

	return MyPS;
}

void AmsCharacter::ServerUpdateCursor_Implementation(float InRotationYaw, float InAimPitch)
{
	SetActorRotation(FRotator(0.f, InRotationYaw, 0.f));
	AimPitch = InAimPitch;
}

void AmsCharacter::InitAbilityActorInfo()
{
	GetMyPS()->GetAbilitySystemComponent()->InitAbilityActorInfo(GetMyPS(), this);
	AbilitySystemComponent = Cast<UmsAbilitySystemComponent>(GetMyPS()->GetAbilitySystemComponent());
	AttributeSet = GetMyPS()->GetAttributeSet();

	if (auto PC = Cast<AmsPlayerController>(GetController()))
	{
		if (AmsHUD* HUD = PC->GetHUD<AmsHUD>())
		{
			HUD->InitOverlay(PC, GetMyPS(), AbilitySystemComponent, AttributeSet);
		}
	}

	TWeakObjectPtr<AmsCharacter> WeakThis(this);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMovementSpeedIncreaseAttribute()).AddLambda(
		[WeakThis](const FOnAttributeChangeData& Data)
		{
			if (AmsCharacter* This = WeakThis.Get())
			{
				if (This->GetCharacterMovement())
				{
					This->GetCharacterMovement()->MaxWalkSpeed = This->DefaultWalkSpped + (This->DefaultWalkSpped * Data.NewValue);
				}
			}
		});

	for (const auto& BuffTag : FmsGameplayTags::Get().BuffTags)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(BuffTag,
			EGameplayTagEventType::AnyCountChange).AddUObject(this, &ThisClass::BuffTagChanged);
	}

	RegisterDebuffTagEvent();

	GetCharacterMovement()->MaxWalkSpeed = GetMovementSpeed();

	InitializeDefaultAttributes();
}

void AmsCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMovementSpeedIncreaseAttribute()).RemoveAll(this);
	for (const auto& BuffTag : FmsGameplayTags::Get().BuffTags)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(BuffTag, EGameplayTagEventType::AnyCountChange).RemoveAll(this);
	}

	AbilitySystemComponent->RegisterGameplayTagEvent(FmsGameplayTags::Get().Debuff_Fire, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	AbilitySystemComponent->RegisterGameplayTagEvent(FmsGameplayTags::Get().Debuff_Ice, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	AbilitySystemComponent->RegisterGameplayTagEvent(FmsGameplayTags::Get().Debuff_Lightning, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	AbilitySystemComponent->RegisterGameplayTagEvent(FmsGameplayTags::Get().Debuff_Poison, EGameplayTagEventType::AnyCountChange).RemoveAll(this);

	Super::EndPlay(EndPlayReason);
}

void AmsCharacter::MulticastHandleRevive_Implementation()
{
	if (!bDead)
	{
		return;
	}

	if (DeadAlret)
	{
		DeadAlret->Destroy();
	}

	if (AmsGameMode* GM = Cast<AmsGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->RequestRespawn(this, GetController());
	}
}
