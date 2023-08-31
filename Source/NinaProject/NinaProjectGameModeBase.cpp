// Copyright Epic Games, Inc. All Rights Reserved.


#include "NinaProjectGameModeBase.h"

ANinaProjectGameModeBase::ANinaProjectGameModeBase() {
	Inventory = CreateDefaultSubobject<UInventory>(TEXT("Inventerer"));
}