// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "msCharacterBase.h"
#include "MidnightSunz/Interface/msArtifactInterface.h"
#include "MidnightSunz/Interface/msPlayerInterface.h"
#include "msCharacter.generated.h"

class USphereComponent;
class USpringArmComponent;
class UCameraComponent;
class AmsPlayerController;
class AmsPlayerState;

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API AmsCharacter : public AmsCharacterBase, public ImsArtifactInterface, public ImsPlayerInterface
{
	GENERATED_BODY()

public:
	AmsCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Controller() override;
	virtual float GetMovementSpeed() const override;
	virtual void Die() override;

	virtual FVector GetCursorLocation() const override { return CursorLocation; };
	virtual float GetAimPitch() const { return AimPitch; };

	// begin ImsCombatInterface
	virtual int32 GetPlayerLevel_Implementation() override;
	// end ImsCombatInterface

	// begin ImsArtifactInterface
	virtual void TriggerKillEnemyArtifact_Implementation(const FVector& TargetLocation) override;
	virtual void TriggerHitEnemyArtifact_Implementation(AActor* TargetActor) override;
	virtual void TriggerNormalAttackArtifact_Implementation() override;
	virtual float GetBaseDamage_Implementation() override;
	virtual float GetProbability_Implementation(const FGameplayTag& InArtifactTag) override;
	virtual bool CheckProbability_Implementation(const FGameplayTag& InArtifactTag) override;
	// end ImsArtifactInterface

	// begin ImsPlayerInterface
	virtual void Revive_Implementation() override;
	// end ImsPlayerInterface

protected:
	virtual void Tick(float DeltaSeconds);
	virtual void InitAbilityActorInfo() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//virtual void MulticastHandleDeath_Implementation() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHandleRevive();

private:
	void ApplySaveData();

	void SetTunrinPlace();
	void TraceUnderCursor(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerUpdateCursor(float InRotationYaw, float InAimPitch);

	UFUNCTION(Client, Reliable)
	void ClientDie();

	AmsPlayerState* GetMyPS();

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<AmsPlayerController> PlayerController;

	UPROPERTY(Transient)
	TObjectPtr<AmsPlayerState> MyPS;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> DeadAlretClass;

	UPROPERTY(Transient, BlueprintReadWrite)
	TObjectPtr<AActor> DeadAlret;

	UPROPERTY(EditAnywhere)
	float RotOffset;

private:
	float CharYaw = 0.f;

	FVector CursorLocation;
	FRotator CurrentLookAtRotation;
	float AimPitch = 0.f;
};