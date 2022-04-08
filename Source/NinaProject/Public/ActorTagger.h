// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActorTagger.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NINAPROJECT_API UActorTagger : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UActorTagger();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bInteraction;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


};
