// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "BaseCharacter.h"
#include "BasePlayerController.generated.h"

enum class EInputState : uint8
{
	Null			UMETA(DisplayName = "Null"),
	Character 		UMETA(DisplayName = "Character"),
	Terminal		UMETA(DisplayName = "Terminal"),
	Movable			UMETA(DisplayName = "Movable"),
	Aim				UMETA(DisplayName = "Aim"),
	UI				UMETA(DisplayName = "UI")
};

/**
 * 
 */
UCLASS()
class NINAPROJECT_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
		float CameraTopLimit;
		float CameraBottomLimit;
		float AngleToRotationThereshold;
		float ZoomDelta;
		float RollDelta;
		FVector2D CameraInput;
		FVector2D Panning;
		FVector DesiredDirection;
		FVector RightMovement;
		FVector ForwardMovement;
		bool bCanMoveSelection;
		bool bDiscreteVerticalInputSent;
		bool bDiscreteHorizontalInputSent;
		bool bAltZoomFunction = false;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ABaseCharacter* BaseCharacter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int YAxisDirection;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsPaused;

	void SetInputState(EInputState NewInputState);

protected:

	EInputState InputState;
	ANWUHUD* HUD;

	void BeginPlay();
	void SetPawnDefaults();
	void CheckRotation();

	void MoveForward(float InValue);
	void MoveRight(float InValue);
	void PanningHor(float InValue);
	void PanningVert(float InValue);
	void ZoomInItem(float AxisValue);
	void ZoomOutItem(float AxisValue);
	void AltTriggers();

	void UpSelection();
	void DownSelection();
	void RightSelection();
	void LeftSelection();
	void Accept();
	void Cancel();

	void Interaction();
	void EndInteraction();
	void Action();
	void AlternativeMovement();
	void AlternativeView();
	void NextPage();
	void PrevPage();
	void Use();

	UFUNCTION(BlueprintCallable, Category = "CodeEvents")
	void SwitchPause();
	UFUNCTION(BlueprintCallable, Category = "CodeEvents")
		void SwitchMenu();
};
