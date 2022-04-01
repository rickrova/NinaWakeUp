// Fill out your copyright notice in the Description page of Project Settings.

#include "NinaCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "BaseMovableActor.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Engine.h"

ANinaCharacter::ANinaCharacter() {
	MinVelocityToJump = 300;
	JumpVelocityFactor = 0.5f;
	FallVelocityFactor = 0.25f;
	CameraFollowThereshold = 3.0f;
	//TimerDel = FTimerDelegate::CreateUObject(this, &ANinaCharacter::ResetJumpValues);
	//TimerDel.BindUFunction(this, FName("ResetJumpValues"));
}

void ANinaCharacter::BeginPlay() {
	Super::BeginPlay();
}

void ANinaCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	/*if (GetMovementComponent()->IsMovingOnGround() && GetVelocity().Size() > 0 && !bPendingJump) {

		FVector CurrentDirection = GetActorForwardVector();
		CurrentDirection.Z = 0;
		float Normal = FVector::CrossProduct(CurrentDirection, LastDirection).Z;
		int Sign = FMath::Abs(Normal) / Normal;
		ZRotationSpeed = FMath::RadiansToDegrees(acosf(FVector::DotProduct(CurrentDirection, LastDirection)));
		LastDirection = CurrentDirection;
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Angle: ")) + FString::SanitizeFloat(ZRotationSpeed));

		FHitResult* HitResult = new FHitResult();
		FVector StartTrace = GetActorLocation()
			+ GetActorForwardVector().GetSafeNormal() * RayOffset;
		FVector DownVector = -FVector::UpVector;
		FVector DownEndTrace = StartTrace + DownVector * RayLength;
		FCollisionQueryParams TraceParams = FCollisionQueryParams::FCollisionQueryParams();

		if (!GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, DownEndTrace, ECC_Visibility, TraceParams)) {

			FVector2D CurrentDirection;
			CurrentDirection.X = GetActorForwardVector().X;
			CurrentDirection.Y = GetActorForwardVector().Y;

			float DirectionsAngle = FMath::RadiansToDegrees(acosf(FVector2D::DotProduct(CurrentDirection,
				LastFallDirection)));

			if (GetVelocity().Size() > MinVelocityToJump && DirectionsAngle <= 10) {
				//Jump();
			}
			LastFallDirection.X = GetActorForwardVector().X;
			LastFallDirection.Y = GetActorForwardVector().Y;
		}
	}*/

	if (bBased) {
		Speed = BaseActor->GetVelocity().Size();
		bReverse = BaseActor->bReverse;
		DesiredPushAngle = BaseActor->Angle;
		DesiredRotation = CameraSpringArm->GetComponentRotation();
		DesiredRotation.Yaw = GetActorRotation().Yaw;
		CameraSpringArm->SetWorldRotation(FMath::Lerp(CameraSpringArm->GetComponentRotation(),
			DesiredRotation, DeltaTime * 5.0f));
		if (Speed > 0) {
			CurrentPushAngle = FMath::Lerp(CurrentPushAngle, DesiredPushAngle * BaseActor->AngleDirection, DeltaTime * PushRotationSpeed);
		}
		//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::White, FString::SanitizeFloat(PushAngle));
		if (BaseActor->bInterruptInteraction) {
			EndInteraction();
			//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Stop Push")));
		}
	}
	else if (bGrab) {
		Speed = GetVelocity().Size();
		DesiredRotation = CameraSpringArm->GetComponentRotation();
		DesiredRotation.Yaw = GetActorRotation().Yaw;
		CameraSpringArm->SetWorldRotation(FMath::Lerp(CameraSpringArm->GetComponentRotation(),
			DesiredRotation, DeltaTime * 5.0f));
	}
	else if (bFloatUp) {
		if (bRotatingLeft) {
			FRotator NewRot = GetActorRotation();
			NewRot.Roll = FMath::Lerp(NewRot.Roll, FMath::Clamp(-DirectionAngle, -JetpackMaxRoll, JetpackMaxRoll), DeltaTime * 1);
			SetActorRotation(NewRot); //FMath::Lerp(GetActorRotation(), NewRot, DeltaTime * 5));
		}
		else if (bRotatingRight) {
			FRotator NewRot = GetActorRotation();
			NewRot.Roll = FMath::Lerp(NewRot.Roll, FMath::Clamp(DirectionAngle, -JetpackMaxRoll, JetpackMaxRoll), DeltaTime * 1);
			SetActorRotation(NewRot); //FMath::Lerp(GetActorRotation(), NewRot, DeltaTime * 5));
		}

		//if (InputVector.Size() > 0) {
		FRotator ForwardRotation = GetActorRotation();
		//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red,
			//FString::SanitizeFloat(FMath::Cos(FMath::DegreesToRadians(DirectionAngle))));
		ForwardRotation.Pitch = FMath::Lerp(ForwardRotation.Pitch, -FMath::Clamp(FMath::Cos(FMath::DegreesToRadians(DirectionAngle)) * 60,
			-JetpackMaxRoll, JetpackMaxRoll), DeltaTime * 1);
		SetActorRotation(ForwardRotation);
		//}
	}
	else {
		Speed = GetVelocity().Size();
		if (GetVelocity().Z < 0) {
			bReverse = true;
		}

		if (Speed > 0) {
			DesiredRotation = CameraSpringArm->GetComponentRotation();
			if (FMath::Abs(GetActorRotation().Yaw - DesiredRotation.Yaw) < 170.0f) {
				DesiredRotation.Yaw = GetActorRotation().Yaw;
				DesiredRotation.Pitch = -25;
				if (!bFixedCamera && !bZoomIn) {
					CameraSpringArm->SetWorldRotation(FMath::Lerp(CameraSpringArm->GetComponentRotation(),
						DesiredRotation, DeltaTime * CameraFollowThereshold));//1.8f));
				}
			}
		}
		if (bCameraReset) {
			DesiredRotation = CameraSpringArm->GetComponentRotation();
			DesiredRotation.Yaw = GetActorRotation().Yaw;
			DesiredRotation.Pitch = 0;
			CameraSpringArm->SetWorldRotation(FMath::Lerp(CameraSpringArm->GetComponentRotation(),
				DesiredRotation, DeltaTime * 10.0f));
			if (FMath::Abs(DesiredRotation.Yaw - CameraSpringArm->GetComponentRotation().Yaw) < 0.5f) {
				bCameraReset = false;
			}
		}
		FRotator NewRot = GetActorRotation();
		NewRot.Roll = FMath::Lerp(NewRot.Roll, 0.0f, DeltaTime * 2.5f);
		NewRot.Pitch = FMath::Lerp(NewRot.Pitch, 0.0f, DeltaTime * 2.5f);
		SetActorRotation(NewRot);
	}

	if (!bFloatUp && JetpackEnergy < JetpackMaxEnergy) {
		if (bJetpackSlowRecover) {
			JetpackEnergy += JetpackEnergyRecoverSpeed * JetpackSlowEnergyRecover * DeltaTime;
		}
		else {
			JetpackEnergy += JetpackEnergyRecoverSpeed * DeltaTime;
		}
		if (JetpackEnergy >= JetpackMaxEnergy) {
			JetpackEnergy = JetpackMaxEnergy;
		}
	}
	//Speed = GetVelocity().Size();

	/*DrawDebugPoint(GetWorld(),
		GetActorLocation(),
		10.0f,
		FColor(255, 0, 0),
		false,
		-1,
		SDPG_MAX);

	DrawDebugPoint(GetWorld(),
		CameraSpringArm->GetComponentLocation(),
		5.0f,
		FColor(0, 255, 0),
		false,
		-1,
		SDPG_MAX);*/
		/*if (bFloatUp && JetpackEnergy > 0) {
			JetpackEnergy -= JetpackEnergyDecreaseSpeed * DeltaTime;
			GetCharacterMovement()->Velocity.Z += JetpackForce * DeltaTime;
		}*/

	if (bJetpackBoost) {
		//GetMovementComponent()->Velocity.Z += 500;
		if (JetpackEnergy > 0) {
			//GetCharacterMovement()->RotationRate = FRotator(0, 0, JetpackRotationSpeed);
			JetpackEnergy -= JetpackEnergyDecreaseSpeed * DeltaTime * 4;
			GetCharacterMovement()->Velocity.Z += (JetpackForce * JetpackBoostPower * UGameplayStatics::GetGlobalTimeDilation(GetWorld()));
		}
		else {
			//OnNinaEndFloat();
			//bFloatUp = false;
			bJetpackBoost = false;
		}
	}

	if (bJetpackSlowRecover && bFloatUp && GetCharacterMovement()->Velocity.Z < 0) {
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::White, FString("Boost fall"));
		OnNinaEndFloat();
		bFloatUp = false;
	}
}

void ANinaCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode,
	uint8 PreviousCustomMode) {
	if (!bClimbing) {
		if (PrevMovementMode == EMovementMode::MOVE_Falling
			&& GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking) {
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::White, FString("Landing"));
			//GetController()->SetIgnoreMoveInput(false);
			OnNinaEndJump();
			OnNinaEndFloat();
			bPendingJump = false;
			bFloatUp = false;
			bJetpackSlowRecover = false;
			//GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 1.f, false);
		}
		else if (PrevMovementMode == EMovementMode::MOVE_Walking
			&& GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling
			&& !bPendingJump) {
			//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::White, FString("Falling"));
			OnFallingDelegate.Broadcast();
			GetCharacterMovement()->Velocity = FVector(GetVelocity().X * FallVelocityFactor,
				GetVelocity().Y * FallVelocityFactor, GetVelocity().Z);
			GetCharacterMovement()->RotationRate = FRotator::ZeroRotator;
		}
	}
}

/*void ANinaCharacter::Climb(FHitResult HitResult) {
	FVector ClimbableOrigin;
	FVector ClimbableExtent;
	FVector DesiredLocation;
	HitResult.GetActor()->GetActorBounds(true, ClimbableOrigin, ClimbableExtent);
	DesiredLocation.X = HitResult.Location.X;
	DesiredLocation.Y = HitResult.Location.Y;
	DesiredLocation.Z = ClimbableOrigin.Z + ClimbableExtent.Z + 120;
	SetActorLocation(DesiredLocation, false, nullptr, ETeleportType::TeleportPhysics);
}*/

void ANinaCharacter::Push(FHitResult HitResult) {
	bBased = true;
	BaseActor = (ABaseMovableActor*)HitResult.GetActor();
	//BaseActor->SetPushingActor(this);
}

void ANinaCharacter::StopPush() {
	bBased = false;
	/*bReverse = false;
	BaseActor->RemovePushingActor();*/
}

void ANinaCharacter::JumpNow() {
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, FString("Jump Now from code"));
	GetCharacterMovement()->Velocity = JumpInitialVelocity;
	GetCharacterMovement()->DoJump(true);
}

void ANinaCharacter::CancelMove() {
	GetController()->SetIgnoreMoveInput(true);
}

void ANinaCharacter::ResetMove() {
	bPendingJump = false;
	bJumping = false;
	bClimbing = false;
	CameraSpringArm->SetRelativeLocation(InitialSpringArmOffset);
	if (!bKO) {
		GetController()->SetIgnoreMoveInput(false);
	}
}

void ANinaCharacter::Jump() {
	//Super::Jump();
	APlayerController* PlayerController = (APlayerController*)GetController();
	if (GetMovementComponent()->IsMovingOnGround() && !bPendingJump && !PlayerController->IsPaused()) {
		JumpInitialVelocity = GetVelocity() * JumpVelocityFactor;
		//PlayerController->SetIgnoreMoveInput(true);
		//bPendingJump = true;
		OnNinaBeginJump();
	}

	if (bCanFloat && GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling && !PlayerController->IsPaused()) {
		//GetMovementComponent()->Velocity.Z += 500;
		bFloatUp = true;
		OnNinaFloat();
	}
}

void ANinaCharacter::FloatUp(float DeltaTime) {

	//GetMovementComponent()->Velocity.Z += 500;
	if (!bJetpackBoost) {
		if (bFloatUp && JetpackEnergy > 0) {
			//GetCharacterMovement()->RotationRate = FRotator(0, 0, JetpackRotationSpeed);
			JetpackEnergy -= JetpackEnergyDecreaseSpeed * DeltaTime;
			GetCharacterMovement()->Velocity.Z += (JetpackForce * UGameplayStatics::GetGlobalTimeDilation(GetWorld()));
		}
		else {
			OnNinaEndFloat();
			bFloatUp = false;
		}
	}
}

void ANinaCharacter::StopJump() {
	OnNinaEndFloat();
	bFloatUp = false;
}

void ANinaCharacter::JetpackBoost() {
	APlayerController* PlayerController = (APlayerController*)GetController();
	if (JetpackEnergy == JetpackMaxEnergy && !PlayerController->IsPaused() && bCanFloat) {
		GetCharacterMovement()->Velocity = JumpInitialVelocity;
		GetCharacterMovement()->DoJump(true);
		bFloatUp = true;
		bJetpackBoost = true;
		bJetpackSlowRecover = true;
		OnNinaFloat();
	}
}

/*void ANinaCharacter::ResetJumpValues() {
	GetCharacterMovement()->JumpZVelocity = initialJumpSpeed;
}*/
