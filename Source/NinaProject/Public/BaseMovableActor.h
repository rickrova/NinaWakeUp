// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/ArrowComponent.h"
#include "BaseMovableActor.generated.h"

UCLASS()
class NINAPROJECT_API ABaseMovableActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseMovableActor();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* VisibleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UArrowComponent* Forward;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* Handle;
	UPROPERTY(BlueprintReadWrite)
		bool bReverse;
	UPROPERTY(EditAnywhere)
		bool bClimbable;
	UPROPERTY(EditAnywhere)
		float ZRotationSpeed;
	UPROPERTY(EditAnywhere)
		float ForceMultiplier;
	UPROPERTY(EditAnywhere)
		float LinearVelocityThereshold;
	UPROPERTY(EditAnywhere)
		float AngularVelocityThereshold;
	UPROPERTY(EditAnywhere)
		FVector StartTraceOffset;
	UPROPERTY(EditAnywhere)
		float TraceLength;

	FHitResult Hit;

protected:

	FVector RightAxis;
	FVector FrontAxis;
	/*float RightForce;
	float FrontForce;*/
	float ZDeltaRotation;
	float MaxReverseAngle;
	bool bCanSwitchDirection;
	int CurrentDirection;
	bool bInteracting;
	FHitResult* FloorHit;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	float Angle;
	bool bInterruptInteraction;
	float AngleDirection;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void OnAddRightMovementInput(FVector Direction, float Value, float DeltaTime);
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void OnAddForwardMovementInput(FVector Direction, float Value, float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "CodeEvents")
		void OnInteraction(FHitResult InHit);
	UFUNCTION(BlueprintCallable, Category = "CodeEvents")
		void OnInteractionEnd();

	void AddRightMovementInput(FVector Direction);
	void AddForwardMovementInput(FVector Direction);
	void SnapToMagnet(FVector NewLocation);
	bool CheckFloor(FVector InOrigin, float InLength);

};
