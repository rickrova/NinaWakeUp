// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NWUHUD.h"
#include "Inventory.generated.h"

USTRUCT(BlueprintType)
struct FItemMeta {
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FName Name;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NINAPROJECT_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:
	TArray<FItemMeta> Items;

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

	void AddToInventory(FItemMeta inItem);
};
