// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "msInputConfig.h"
#include "msEnhancedInputComponent.generated.h"

/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API UmsEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
	void BindAbilityActions(const UmsInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);
};

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
inline void UmsEnhancedInputComponent::BindAbilityActions(const UmsInputConfig* InputConfig,
	UserClass* Object,
	PressedFuncType PressedFunc,
	ReleasedFuncType ReleasedFunc,
	HeldFuncType HeldFunc)
{
	check(InputConfig);

	for (const auto& InputAction : InputConfig->AbilityInputActions)
	{
		if (InputAction.InputAction && InputAction.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(InputAction.InputAction, ETriggerEvent::Started, Object, PressedFunc, InputAction.InputTag);
			}
			if (ReleasedFunc)
			{
				BindAction(InputAction.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, InputAction.InputTag);
			}
			if (HeldFunc)
			{
				BindAction(InputAction.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, InputAction.InputTag);
			}
		}
	}
}
