// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "HumanAnimInstance.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h"
#include "Inventory.h"
#include "BaseCharacter.generated.h"

UENUM(BlueprintType)
enum class EInputType : uint8
{
	Interaction 	UMETA(DisplayName = "Interaction"),
	Action			UMETA(DisplayName = "Action")
};

UENUM(BlueprintType)
enum class EInteractionType : uint8 {
	Push UMETA(DisplayName = "Push"),
	Grab UMETA(DisplayNAme = "Grab")
};

UCLASS()
class NINAPROJECT_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	FVector DesiredDirection;
	FVector InputVector;
	FVector InitialSpringArmOffset;
	float InitialSpringArmLength;
	bool bCameraTriggered;
	bool bWakeUp;
	bool bGrab;
	bool bCameraReset;

	UPROPERTY(BlueprintReadOnly)
	UCameraComponent* CurrentCamera;

	UPROPERTY(EditAnywhere)
		float RayOffset;
	UPROPERTY(EditAnywhere)
		float FrontRayOffset;
	UPROPERTY(EditAnywhere)
		float VerticalRayOffset;
	UPROPERTY(EditAnywhere)
		float RayLength;
	UPROPERTY(EditAnywhere)
		float FrontRayLength;
	UPROPERTY(EditAnywhere)
		float VerticalRayLength;
	UPROPERTY(EditAnywhere)
		float CameraTransitionYawThreshold;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bBased;
	/*UPROPERTY(BlueprintReadOnly)
		FHitResult ExposedHit;*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class ABaseMovableActor* BaseActor;
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector DesiredDirection;*/
	UPROPERTY(EditDefaultsOnly)
		UCurveFloat* EaseInOutCurve;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bRotatingRight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bRotatingLeft;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float DirectionAngle;
	UPROPERTY(EditAnywhere)
		float GrabWalkSpeed;
	UPROPERTY(EditAnywhere)
		float AimWalkSpeed;
	UPROPERTY(BlueprintReadOnly)
		bool bZoomIn;
	UPROPERTY(BlueprintReadWrite)
		bool bAimGadget;
	UPROPERTY(BlueprintReadWrite)
		bool bPendingJump;
	UPROPERTY(BlueprintReadWrite)
		bool bCanFloat;
	UPROPERTY(EditAnywhere)
		float JetpackForce;
	UPROPERTY(EditAnywhere)
		float JetpackEnergyRecoverSpeed;
	UPROPERTY(EditAnywhere)
		float JetpackEnergyDecreaseSpeed;
	UPROPERTY(BlueprintReadOnly)
		float JetpackEnergy;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float JetpackMaxEnergy;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float JetpackRotationSpeed;
	UPROPERTY(EditAnywhere)
		float JetpackMaxRoll;
	UPROPERTY(EditAnywhere)
		float JetpackBoostPower;
	UPROPERTY(EditAnywhere)
		float JetpackSlowEnergyRecover;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int LifeUnits;
	UPROPERTY(BlueprintReadWrite)
		bool bInvincible;
	UPROPERTY(BlueprintReadWrite)
		bool bKO;


protected:
	UPROPERTY(BlueprintReadOnly)
		UCameraComponent* PendingCamera;
	UHumanAnimInstance* AnimInstance;
	UTimelineComponent* ClimbFixPositionTimeline;
	UTimelineComponent* FixPositionTimeline;
	UTimelineComponent* FirstPersonCameraTransitionTimeline;
	UTimelineComponent* ZoomInCameraTransitionTimeline;
	UTimelineComponent* TransparentTransitionTimeline;
	FOnTimelineFloat ClimbFixPositionCallback{};
	FOnTimelineFloat FixPositionCallback{};
	FOnTimelineFloat FirstPersonCameraTransitionCallback{};
	FOnTimelineFloat ZoomInCameraTransitionCallback{};
	FOnTimelineFloat TransparentTransitionCallback{};
	FVector DesiredLocation;
	FRotator DesiredRotation;
	FVector InitialLocation;
	FRotator InitialRotation;
	FVector ClimbOffset;
	FRotator SpringArmRotationAfterFirstPersonCamera;
	bool bIsFirstPerson;
	UMaterialInstanceDynamic* GeneralMaterial;
	UMaterialInstanceDynamic* HairMaterial;
	bool bClimbing;
	FRotator InitialRotationRate;
	float InitialWalkSpeed;
	float CameraTransitionYaw;
	bool bIsHide;
	bool bUI;
	bool bJetpackBoost;
	bool bJetpackSlowRecover;

	UPROPERTY(BlueprintReadOnly)
		bool bFloatUp;

	UPROPERTY(EditAnywhere)
		float PushRotationSpeed;
	UPROPERTY(EditAnywhere)
		float ClimbOffsetForwardThereshold;
	UPROPERTY(EditAnywhere)
		FVector ZoomInCameraOffset;
	UPROPERTY(EditAnywhere)
		float MinTransparency;

	virtual void BeginPlay() override;
	void Climb(FHitResult* HitResult);
	void FixPosition(FHitResult* HitResult, bool bClimb);
	virtual void Jump();
	void GetCurvePosition();

public:
	virtual void Tick(float DeltaSeconds) override;
	//void ClearInteractable();
	void CheckFrontObject(EInputType InputType);
	void SetAltMovement();
	void SetZoomInCamera();
	void SetZoomOutCamera();

	virtual void FloatUp(float DeltaTime);
	virtual void StopJump();
	virtual void JetpackBoost();

	//void Use();

	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void Interaction();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void StartInteraction(AActor* InteractiveActor, FHitResult HitResult);
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void EndInteraction();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void Action();
	UFUNCTION(BlueprintCallable, Category = "CodeEvents")
		void OnInteraction(FHitResult HitResult);
	UFUNCTION(BlueprintCallable, Category = "CodeEvents")
		void OnInteractionEnd();
	UFUNCTION(BlueprintCallable, Category = "CodeEvents")
		void OnAction();
	UFUNCTION(BlueprintCallable, Category = "CodeEvents")
		void OnClimbEnd();
	UFUNCTION(BlueprintCallable, Category = "CodeFunction")
		void OnGrab(FHitResult HitResult);
	UFUNCTION(BlueprintCallable, Category = "CodeFunction")
		void OnGrabEnd();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void OnFixPositionEnd();
	UFUNCTION()
		void ClimbFixPositionTimelineCallback();
	UFUNCTION()
		void FixPositionTimelineCallback();
	UFUNCTION()
		void FirstPersonCameraTimelineCallback();
	UFUNCTION()
		void TransparentTimelineCallback();
	UFUNCTION()
		void ZoomInCameraTimelineCallback();

	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void UpSelection();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void DownSelection();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void AcceptSelection();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void Pause(bool bPause);
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void NinaWakeUp();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void ShowUI(int TextureIndex);
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void HideUI();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void Use();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void Zoom(bool bZoom);

	UFUNCTION(BlueprintCallable, Category = "CodeFunction")
		void SetFirstPersonCamera(bool InValue);
	UFUNCTION(BlueprintCallable, Category = "CodeFunction")
		void HideNina(bool InValue);
	UFUNCTION(BlueprintCallable, Category = "CodeFunction")
		void BlinkMaterial();
	UFUNCTION(BlueprintCallable, Category = "CodeFunction")
		void SetCamera(UCameraComponent* inCamera);
};
