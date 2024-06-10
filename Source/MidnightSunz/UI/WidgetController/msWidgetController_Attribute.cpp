// Fill out your copyright notice in the Description page of Project Settings.


#include "msWidgetController_Attribute.h"
#include "MidnightSunz/AbilitySystem/Attributes/msAttributeSet.h"
#include "MidnightSunz/AbilitySystem/msAbilitySystemComponent.h"

void UmsWidgetController_Attribute::BindCallbacksToDependencies()
{
	for (const auto& Pair : AttributeSet->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda
		(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				AttributeInfoDelegate.Broadcast(Pair.Key, Pair.Value().GetNumericValue(AttributeSet));
			}
		);
	}
}

void UmsWidgetController_Attribute::BroadcastInitialValues()
{
	for (const auto& Pair : AttributeSet->TagsToAttributes)
	{
		AttributeInfoDelegate.Broadcast(Pair.Key, Pair.Value().GetNumericValue(AttributeSet));
	}
}
