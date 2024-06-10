// Fill out your copyright notice in the Description page of Project Settings.


#include "msEnemy.h"
#include "MidnightSunz/msGameplayTags.h"
#include "MidnightSunz/GameModes/msGameState.h"
#include "MidnightSunz/AbilitySystem/msAbilitySystemLibrary.h"
#include "MidnightSunz/AbilitySystem/msAbilitySystemComponent.h"
#include "MidnightSunz/AbilitySystem/Attributes/msAttributeSet.h"
#include "MidnightSunz/UI/Widgets/msUserWidget.h"
#include "MidnightSunz/AI/msAIController.h"
#include "Components/WidgetComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

AmsEnemy::AmsEnemy()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UmsAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UmsAttributeSet>(TEXT("AttributeSet"));

	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(GetRootComponent());

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void AmsEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	msAIController = Cast<AmsAIController>(NewController);
	msAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->GetBlackboardAsset());

	if (bIsSpawnFinished)
	{
		msAIController->RunBehaviorTree(BehaviorTree);
	}

	msAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	msAIController->GetBlackboardComponent()->SetValueAsBool(FName("Electrocuted"), false);
	bRangedAttacker = CharacterClass == ECharacterType::MainPlant || CharacterClass == ECharacterType::Lich_Ranged;
	msAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), bRangedAttacker);

	if (bIsSpawnFinished)
	{
		msAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsFinishSpawnAnim"), true);
	}
}

void AmsEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AmsEnemy, bIsSpawnFinished);
}

int32 AmsEnemy::GetPlayerLevel_Implementation()
{
	return Level;
}

void AmsEnemy::BP_Die_Implementation()
{
	Die();
}

void AmsEnemy::Die()
{
	SetLifeSpan(LifeSpan);

	if (msAIController)
	{
		msAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
	}

	Super::Die();
}

void AmsEnemy::SetIsBeingElectrocuted_Implementation(bool bInElectrocute)
{
	Super::SetIsBeingElectrocuted_Implementation(bInElectrocute);

	if (msAIController)
	{
		msAIController->GetBlackboardComponent()->SetValueAsBool(FName("Electrocuted"), bInElectrocute);
	}
}

void AmsEnemy::BeginPlay()
{
	Super::BeginPlay();
	PlaySpanwMontage();

	InitAbilityActorInfo();
	GetCharacterMovement()->MaxWalkSpeed = GetMovementSpeed();

	Level = Cast<AmsGameState>(UGameplayStatics::GetGameState(this))->GetEnemyLevel();

	if (HasAuthority())
	{
		InitializeDefaultAttributes();
		UmsAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
	}

	if (UmsUserWidget* HealthBarWidget = Cast<UmsUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		HealthBarWidget->SetWidgetController(this);
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChange.Broadcast(Data.NewValue);
		});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChange.Broadcast(Data.NewValue);
		});

	AbilitySystemComponent->RegisterGameplayTagEvent(FmsGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::HitReactTagChanged);

	RegisterDebuffTagEvent();

	OnHealthChange.Broadcast(AttributeSet->GetHealth());
	OnMaxHealthChange.Broadcast(AttributeSet->GetMaxHealth());
}

void AmsEnemy::PlaySpanwMontage()
{
	if (bIsSpawnFinished)
	{
		return;
	}

	if (!SpawnMontage)
	{
		FinishSpawnMontage();
		return;
	}

	SetActorHiddenInGame(true);

	auto TimerCallback = [this]()
		{
			SetActorHiddenInGame(false);
		};
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda(TimerCallback);
	GetWorld()->GetTimerManager().SetTimer(SpawnHideTimerHanlde, this, &ThisClass::ShowActorVisible, 0.3f, false);

	const float PlayLengh = GetMesh()->GetAnimInstance()->Montage_Play(SpawnMontage);
	const float Time = FMath::Max(PlayLengh, 0.1f);
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHanlde, this, &ThisClass::FinishSpawnMontage, Time, false);

	GetMesh()->TickAnimation(0.f, false);
	GetMesh()->RefreshBoneTransforms();

}

void AmsEnemy::ShowActorVisible()
{
	SetActorHiddenInGame(false);
}

void AmsEnemy::FinishSpawnMontage()
{
	bIsSpawnFinished = true;
	if (msAIController && msAIController->GetBlackboardComponent())
	{
		msAIController->RunBehaviorTree(BehaviorTree);

		msAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsFinishSpawnAnim"), true);
	}
}

void AmsEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AmsEnemy::InitializeDefaultAttributes() const
{
	UmsAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void AmsEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReact = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReact ? 0.f : GetMovementSpeed();
	if (msAIController && msAIController->GetBlackboardComponent())
	{
		msAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReact);
	}
}
