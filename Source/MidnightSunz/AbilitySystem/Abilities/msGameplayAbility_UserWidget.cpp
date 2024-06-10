// Fill out your copyright notice in the Description page of Project Settings.


#include "msGameplayAbility_UserWidget.h"

UmsGameplayAbility_UserWidget::UmsGameplayAbility_UserWidget()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
}

void UmsGameplayAbility_UserWidget::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
