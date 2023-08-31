// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Inventory.h"
#include "NinaProjectGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class NINAPROJECT_API ANinaProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInventory* Inventory;

	ANinaProjectGameModeBase();
};
