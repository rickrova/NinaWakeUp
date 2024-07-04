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
	void UpdateInventory(FItemMeta inItem);
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void SwitchInventoryVisibility();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void SwitchPanelVisibility(int InTypeID, int OutTypeID);
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void CloseInspectMesh();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void Right();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void Left();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void Up();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void Down();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void NextPage();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void PrevPage();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void Accept();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void Cancel();

	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void Vertical(float Value);
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void Horizontal(float Value);
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void ZoomItem(float Value);
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void RollItem(float Value);
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void PanItem(FVector2D Value);
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void MoveCursorVertical(float Value);
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void MoveCursorHorizontal(float Value);
};
