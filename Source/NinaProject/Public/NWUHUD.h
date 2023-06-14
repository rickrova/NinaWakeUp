// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NWUHUD.generated.h"

/**
 * 
 */
UCLASS()
class NINAPROJECT_API ANWUHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
	void UpdateInventory();
};
