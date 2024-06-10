// Fill out your copyright notice in the Description page of Project Settings.


#include "msCharacterClassInfo.h"

FCharacterClassDefaultInfo UmsCharacterClassInfo::GetClassDefaultInfo(ECharacterType CharacterClass)
{
	return CharacterClassInformation.FindChecked(CharacterClass);
}
