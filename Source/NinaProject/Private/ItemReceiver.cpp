// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemReceiver.h"
#include "NinaCharacter.h"

UItemReceiver::UItemReceiver()
{
	PrimaryComponentTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	//Collision->SetupAttachment(GetOwner()->GetRootComponent());
}


void UItemReceiver::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(this, &UItemReceiver::OnOverlapBegin);
}


void UItemReceiver::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UItemReceiver::OnOverlapBegin(class UPrimitiveComponent* ThisComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult)
{
	ANinaCharacter* Nina = Cast<ANinaCharacter>(OtherActor);
	if (Nina) {
	}
}

