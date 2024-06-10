// Fill out your copyright notice in the Description page of Project Settings.


#include "msArtifact.h"

void UmsArtifact::InitArtifact(const FGameplayTag& InTag, const int32 InStack)
{
	ArtifactTag = InTag;
	StackCount = InStack;
}

float UmsArtifact::GetProbability() const
{
	return BaseValue * StackCount;
}

bool UmsArtifact::CheckProbability() const
{
	return  FMath::FRandRange(0.f, 1.f) < BaseValue * StackCount;
}
