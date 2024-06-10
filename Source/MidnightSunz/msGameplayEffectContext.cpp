// Fill out your copyright notice in the Description page of Project Settings.


#include "msGameplayEffectContext.h"

bool FmsGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	uint8 RepBits = 0;
	if (Ar.IsSaving())
	{
		if (bReplicateInstigator && Instigator.IsValid())
		{
			RepBits |= 1 << 0;
		}
		if (bReplicateEffectCauser && EffectCauser.IsValid())
		{
			RepBits |= 1 << 1;
		}
		if (AbilityCDO.IsValid())
		{
			RepBits |= 1 << 2;
		}
		if (bReplicateSourceObject && SourceObject.IsValid())
		{
			RepBits |= 1 << 3;
		}
		if (Actors.Num() > 0)
		{
			RepBits |= 1 << 4;
		}
		if (HitResult.IsValid())
		{
			RepBits |= 1 << 5;
		}
		if (bHasWorldOrigin)
		{
			RepBits |= 1 << 6;
		}
		if (bIsCriticalHit)
		{
			RepBits |= 1 << 7;
		}
		if (bIsSuccessfulDebuff)
		{
			RepBits |= 1 << 8;
		}
		if (DebuffDamage > 0.f)
		{
			RepBits |= 1 << 9;
		}
		if (DebuffDuration > 0.f)
		{
			RepBits |= 1 << 10;
		}
		if (DebuffFrequency > 0.f)
		{
			RepBits |= 1 << 11;
		}
		if (DamageType.IsValid())
		{
			RepBits |= 1 << 12;
		}
		if (bIsArtifactDamage)
		{
			RepBits |= 1 << 13;
		}
		if (bIsBlockedDamage)
		{
			RepBits |= 1 << 14;
		}
		if (bIsRadialDamage)
		{
			RepBits |= 1 << 15;
			if (RadialDamageInnerRadius > 0.f)
			{
				RepBits |= 1 << 16;
			}
			if (RadialDamageOuterRadius > 0.f)
			{
				RepBits |= 1 << 17;
			}
			if (!RadialDamageOrigin.IsZero())
			{
				RepBits |= 1 << 18;
			}
		}
	}

	Ar.SerializeBits(&RepBits, 18);

	if (RepBits & (1 << 0))
	{
		Ar << Instigator;
	}
	if (RepBits & (1 << 1))
	{
		Ar << EffectCauser;
	}
	if (RepBits & (1 << 2))
	{
		Ar << AbilityCDO;
	}
	if (RepBits & (1 << 3))
	{
		Ar << SourceObject;
	}
	if (RepBits & (1 << 4))
	{
		SafeNetSerializeTArray_Default<31>(Ar, Actors);
	}
	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = TSharedPtr<FHitResult>(new FHitResult());
			}
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}
	if (RepBits & (1 << 7))
	{
		Ar << bIsCriticalHit;
	}
	if (RepBits & (1 << 8))
	{
		Ar << bIsSuccessfulDebuff;
	}
	if (RepBits & (1 << 9))
	{
		Ar << DebuffDamage;
	}
	if (RepBits & (1 << 10))
	{
		Ar << DebuffDuration;
	}
	if (RepBits & (1 << 11))
	{
		Ar << DebuffFrequency;
	}
	if (RepBits & (1 << 12))
	{
		if (Ar.IsLoading())
		{
			if (!DamageType.IsValid())
			{
				DamageType = TSharedPtr<FGameplayTag>(new FGameplayTag());
			}
		}
		DamageType->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & 1 << 13)
	{
		Ar << bIsArtifactDamage;
	}
	if (RepBits & 1 << 14)
	{
		Ar << bIsBlockedDamage;
	}
	if (RepBits & (1 << 15))
	{
		Ar << bIsRadialDamage;

		if (RepBits & (1 << 16))
		{
			Ar << RadialDamageInnerRadius;
		}
		if (RepBits & (1 << 17))
		{
			Ar << RadialDamageOuterRadius;
		}
		if (RepBits & (1 << 18))
		{
			RadialDamageOrigin.NetSerialize(Ar, Map, bOutSuccess);
		}
	}

	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorAbilitySystemComponent
	}

	bOutSuccess = true;
	return true;
}
