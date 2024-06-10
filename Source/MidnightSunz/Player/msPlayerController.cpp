
// Fill out your copyright notice in the Description page of Project Settings.


#include "msPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "MidnightSunz/Input/msEnhancedInputComponent.h"
#include "MidnightSunz/AbilitySystem/msAbilitySystemComponent.h"
#include "MidnightSunz/Interface/msCombatInterface.h"
#include "MidnightSunz/Interface/msPlayerInterface.h"
#include "MidnightSunz/msGameplayTags.h"
#include "MidnightSunz/GameModes/msGameMode.h"
#include "MidnightSunz/UI/HUD/msHUD.h"
#include "msCheatManager.h"
#include "Net/UnrealNetwork.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlayer.h"
#include "Kismet/GameplayStatics.h"

void AmsPlayerController::ShowDamageNumber_Implementation(ACharacter* TargetCharacter, const FDamageTextParams& DamageTextParams)
{
	if (!IsValid(TargetCharacter) && !IsLocalController())
	{
		return;
	}

	if (ImsCombatInterface* CombatInterface = Cast<ImsCombatInterface>(TargetCharacter))
	{
		CombatInterface->Execute_ShowDamageText(TargetCharacter, DamageTextParams);
	}
}

AmsPlayerController::AmsPlayerController()
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	CheatClass = UmsCheatManager::StaticClass();
#endif
}

void AmsPlayerController::OnStageStateSet(FName State)
{
	StageState = State;
	OnRep_StageState();
}

void AmsPlayerController::OnRep_StageState()
{
	if (StageState == msStageState::StageStart)
	{
		HandleStageStartState();
	}
	else if (StageState == msStageState::GameOver)
	{
		HandleGameOverState();
	}
}

void AmsPlayerController::HandleStageStartState()
{
	bDisableGameplay = false;
}
void AmsPlayerController::HandleGameOverState()
{
	bDisableGameplay = true;
}

void AmsPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(DefaultMappingContext);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);

}

void AmsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UmsEnhancedInputComponent* EnhancedInputComponent = CastChecked<UmsEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAbilityActions(InputConfig,
		this,
		&ThisClass::AbilityInputTagPressed,
		&ThisClass::AbilityInputTagReleased,
		&ThisClass::AbilityInputTagHeld);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Started, this, &ThisClass::InteractionStart);
	EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Completed, this, &ThisClass::InteractionComplete);
}

void AmsPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AmsPlayerController, StageState);
}

void AmsPlayerController::Move(const FInputActionValue& InputActionValue)
{
	if (bDisableGameplay)
	{
		return;
	}

	auto ASC = GetAbilitySystemComponent();
	if (ASC && (ASC->HasMatchingGameplayTag(FmsGameplayTags::Get().Player_Block_InputPressed) || ASC->HasMatchingGameplayTag(FmsGameplayTags::Get().Debuff_Lightning)))
	{
		return;
	}

	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AmsPlayerController::InteractionStart(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		if (ControlledPawn->Implements<UmsPlayerInterface>())
		{
			ImsPlayerInterface::Execute_StartInteraction(ControlledPawn);
		}
	}
}

void AmsPlayerController::InteractionComplete(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		if (ControlledPawn->Implements<UmsPlayerInterface>())
		{
			ImsPlayerInterface::Execute_CompleteInteraction(ControlledPawn);
		}
	}
}

void AmsPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (bDisableGameplay)
	{
		return;
	}

	auto ASC = GetAbilitySystemComponent();
	if (ASC && (ASC->HasMatchingGameplayTag(FmsGameplayTags::Get().Player_Block_InputPressed) || ASC->HasMatchingGameplayTag(FmsGameplayTags::Get().Debuff_Lightning)))
	{
		return;
	}

	if (ASC)
	{
		ASC->AbilityInputTagPressed(InputTag);
	}
}

void AmsPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (bDisableGameplay)
	{
		return;
	}

	auto ASC = GetAbilitySystemComponent();
	if (ASC && (ASC->HasMatchingGameplayTag(FmsGameplayTags::Get().Player_Block_InputReleased) || ASC->HasMatchingGameplayTag(FmsGameplayTags::Get().Debuff_Lightning)))
	{
		return;
	}

	if (ASC)
	{
		ASC->AbilityInputTagReleased(InputTag);
		if (InputTag.MatchesTagExact(FmsGameplayTags::Get().InputTag_LMB))
		{
			bPreseedLMB = false;
		}
	}
}

void AmsPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (bDisableGameplay)
	{
		return;
	}

	auto ASC = GetAbilitySystemComponent();
	if (ASC && (ASC->HasMatchingGameplayTag(FmsGameplayTags::Get().Player_Block_InputHeld) || ASC->HasMatchingGameplayTag(FmsGameplayTags::Get().Debuff_Lightning)))
	{
		return;
	}

	if (ASC)
	{
		ASC->AbilityInputTagHeld(InputTag);
		if (InputTag.MatchesTagExact(FmsGameplayTags::Get().InputTag_LMB))
		{
			bPreseedLMB = true;
		}
	}
}

UmsAbilitySystemComponent* AmsPlayerController::GetAbilitySystemComponent()
{
	if (!AbilitySystemComponent)
	{
		AbilitySystemComponent = Cast<UmsAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AbilitySystemComponent;
}
