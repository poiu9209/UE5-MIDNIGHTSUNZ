// Fill out your copyright notice in the Description page of Project Settings.


#include "msAbilityInfo.h"
#include "MidnightSunz/msLogChannels.h"

FAbilityInfo UmsAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const auto& Info : AbilityInformation)
	{
		if (Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(Logms, Error, TEXT("Can't Find Info for Ability Tag [%s] on AbilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));
	}

	return FAbilityInfo();
}
