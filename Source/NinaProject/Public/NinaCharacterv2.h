// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "NinaCharacterv2.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFallingDelegate2);

UCLASS()
class NINAPROJECT_API ANinaCharacterv2 : public ABaseCharacter
{
	GENERATED_BODY()

		ANinaCharacterv2();
	UPROPERTY(EditAnywhere)
		float MinVelocityToJump;
	UPROPERTY(EditAnywhere)
		float JumpVelocityFactor;
	/*UPROPERTY(EditAnywhere)
		float SecondJumpHeight;
	UPROPERTY(EditAnywhere)
		float SecondJumpThereshold;*/

public:
	//UPROPERTY(EditAnywhere, BlueprintReadOnly)
		//bool bClimbing;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bJumping;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Speed;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
		float FallVelocityFactor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bReverse;
	UPROPERTY(BlueprintReadOnly)
		float ZRotationSpeed;
	UPROPERTY(BlueprintReadOnly)
		float CurrentPushAngle;
	UPROPERTY(EditAnywhere)
		float CameraFollowThereshold;
	UPROPERTY(BlueprintAssignable, Category = "CodeDispatcher")
		FFallingDelegate2 OnFallingDelegate;

private:
	FVector2D LastFallDirection;
	FVector JumpInitialVelocity;
	FVector LastDirection;
	FRotator LastRotationRate;
	float DesiredPushAngle;
	//float initialJumpSpeed;
	//FTimerDelegate TimerDel;
	//FTimerHandle TimerHandle;

	void BeginPlay();
	void Tick(float DeltaSeconds);
	//void ResetJumpValues();

	UFUNCTION()
		virtual void OnMovementModeChanged(EMovementMode PrevMovementMode,
			uint8 PreviousCustomMode);

protected:
	virtual void Jump() override;

public:
	virtual void FloatUp(float DeltaTime) override;
	virtual void StopJump() override;
	virtual void JetpackBoost();

	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void OnNinaBeginJump();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void OnNinaEndJump();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void OnNinaFloat();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void OnNinaEndFloat();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void OnNinaBeginClimb();
	UFUNCTION(BlueprintImplementableEvent, Category = "CodeEvents")
		void OnNinaEndClimb();

	/*UFUNCTION(BlueprintCallable, Category = "CodeEvents")
		void Climb(FHitResult HitResult);*/
	UFUNCTION(BlueprintCallable, Category = "CodeEvents")
		void Push(FHitResult HitResult);
	UFUNCTION(BlueprintCallable, Category = "CodeEvents")
		void StopPush();
	UFUNCTION(BlueprintCallable, Category = "CodeEvents")
		void JumpNow();
	UFUNCTION(BlueprintCallable, Category = "CodeEvents")
		void CancelMove();
	UFUNCTION(BlueprintCallable, Category = "CodeEvents")
		void ResetMove();

};
