// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"

// Sets default values for this component's properties
UInventory::UInventory()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UInventory::BeginPlay()
{
	Super::BeginPlay();
	
}


void UInventory::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UInventory::AddToInventory(FItemMeta inItem) {
	Items.Add(inItem);
	HUD->UpdateInventory(inItem);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Inventory here!")));
}

void UInventory::EquipItem(FItemMeta inItem) {
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Equip now")));
	EquippedItem = inItem;
}

