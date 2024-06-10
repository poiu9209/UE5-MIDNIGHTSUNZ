// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "msAIController.generated.h"

class UBehaviorTreeComponent;
/**
 *
 */
UCLASS()
class MIDNIGHTSUNZ_API AmsAIController : public AAIController
{
	GENERATED_BODY()
public:
	AmsAIController();

protected:
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;


};
