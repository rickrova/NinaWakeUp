// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NWUHUD.h"
#include "Item.h"
#include "Inventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NINAPROJECT_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:
	TArray<FItemMeta> Items;

	UPROPERTY(BlueprintReadWrite)
		FItemMeta EquippedItem;
	UPROPERTY(BlueprintReadWrite)
		ANWUHUD* HUD;

public:	
	// Sets default values for this component's properties
	UInventory();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "CodeFunction")
		void AddToInventory(FItemMeta inItem);
	UFUNCTION(BlueprintCallable, Category = "CodeFunction")
		void EquipItem(FItemMeta inItem);
};
