// Fill out your copyright notice in the Description page of Project Settings.


#include "msAnimInstance.h"
#include "msCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UmsAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AmsCharacterBase>(TryGetPawnOwner());
}

void UmsAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	Character = Character ? Character : Cast<AmsCharacterBase>(TryGetPawnOwner());
	if (!Character)
	{
		return;
	}

	FVector Velocity = Character->GetVelocity();
	Speed = Velocity.Size2D();
	bShouldMove = Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f && Speed > 3.f;
	TurnInPlace = Character->GetTurnInPlace();
	bIsBeingElectrocuted = Character->bIsBeingElectrocuted;

	if (USkeletalMeshComponent* Weapon = Character->Weapon)
	{
		if (Weapon && Weapon->SkeletalMesh && Character->GetMesh())
		{
			LeftHandTransform = Weapon->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
			FVector OutPos;
			FRotator OutRot;
			Character->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPos, OutRot);
			LeftHandTransform.SetLocation(OutPos);
			LeftHandTransform.SetRotation(FQuat(OutRot));

			if (Character->IsLocallyControlled())
			{
				FTransform RightHandTransform = Weapon->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - Character->GetCursorLocation()));
				RightHandRotaion = FMath::RInterpTo(RightHandRotaion, LookAtRotation, DeltaTime, 30.f);
			}

			AimPitch = Character->GetAimPitch();
		}
	}
}
