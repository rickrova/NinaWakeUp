// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorTagger.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"

UActorTagger::UActorTagger()
{
	PrimaryComponentTick.bCanEverTick = false;
}

//este chavo

void UActorTagger::BeginPlay()
{
	Super::BeginPlay();
	if (bAction) {
		GetOwner()->Tags.Add(FName("Action"));
	}
	if (bInteraction) {
		GetOwner()->Tags.Add(FName("Interaction"));
	}
}

