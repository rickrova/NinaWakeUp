// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "NinaCharacter.h"
#include "../NinaProjectGameModeBase.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visible"));

	SetRootComponent(Collision);
	//Collision->SetupAttachment(Root);
	VisibleComponent->SetupAttachment(Collision);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	//Collision->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnOverlapBegin);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/*void AItem::OnOverlapBegin(class UPrimitiveComponent* thisComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult)
{
	ANinaCharacter* Nina = Cast<ANinaCharacter>(OtherActor);
	if (Nina) {
		ANinaProjectGameModeBase* gmb = GetWorld()->GetAuthGameMode<ANinaProjectGameModeBase>();
		UInventory* inventory = gmb->Inventory;

		FItemMeta newItem;
		newItem.Name = FName(TEXT("item"));
		inventory->AddToInventory(newItem);
		//Destroy();
	}

}*/
