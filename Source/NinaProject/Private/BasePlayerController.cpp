// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerController.h"
#include "BaseMovableActor.h"
#include "Engine.h"
#include "Runtime/Core/Public/Misc/App.h"


void ABasePlayerController::BeginPlay() {
	Super::BeginPlay(); 
	SetPawnDefaults();
	/*CharacterCamera = Nina->Camera;
	CharacterCameraSpringArm = Nina->CameraSpringArm;*/
	//SetPawnDefaults();
	//CameraTopLimit = BaseCharacter->CameraTopLimit;
	//CameraBottomLimit = BaseCharacter->CameraBottomLimit;
	AngleToRotationThereshold = 15;
	YAxisDirection = -1;

	InputComponent->BindAxis("MoveForward", this, &ABasePlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABasePlayerController::MoveRight);
	InputComponent->BindAxis("CameraPitch", this, &ABasePlayerController::PitchCamera);
	InputComponent->BindAxis("CameraYaw", this, &ABasePlayerController::YawCamera);

	InputComponent->BindAction("Interact", IE_Pressed, this, &ABasePlayerController::Interaction);
	InputComponent->BindAction("Interact", IE_Released, this, &ABasePlayerController::EndInteraction);
	InputComponent->BindAction("Action", IE_Pressed, this, &ABasePlayerController::Action);
	InputComponent->BindAction("AlternativeMovement", IE_Pressed, this, &ABasePlayerController::AlternativeMovement);
	InputComponent->BindAction("AlternativeView", IE_Pressed, this, &ABasePlayerController::AlternativeView);
	InputComponent->BindAction("Aim", IE_Pressed, this, &ABasePlayerController::Aim);
	InputComponent->BindAction("Aim", IE_Released, this, &ABasePlayerController::EndAim);
	InputComponent->BindAction("Use", IE_Pressed, this, &ABasePlayerController::Use);
	InputComponent->BindAction("Pause", IE_Pressed, this, &ABasePlayerController::SwitchPause).bExecuteWhenPaused = true;

	InputComponent->BindAction("UpSelection", IE_Pressed, this, &ABasePlayerController::UpSelection).bExecuteWhenPaused = true;
	InputComponent->BindAction("DownSelection", IE_Pressed, this, &ABasePlayerController::DownSelection).bExecuteWhenPaused = true;
	InputComponent->BindAction("RightSelection", IE_Pressed, this, &ABasePlayerController::RightSelection).bExecuteWhenPaused = true;
	InputComponent->BindAction("LeftSelection", IE_Pressed, this, &ABasePlayerController::LeftSelection).bExecuteWhenPaused = true;
	InputComponent->BindAction("Accept", IE_Pressed, this, &ABasePlayerController::Accept).bExecuteWhenPaused = true;
	InputComponent->BindAction("Cancel", IE_Pressed, this, &ABasePlayerController::Cancel).bExecuteWhenPaused = true;
	//SwitchPause();

	bDiscreteHorizontalInputSent = true;
	bDiscreteVerticalInputSent = true;

	SetInputState(EInputState::Character);
}

void ABasePlayerController::SetPawnDefaults() {
	BaseCharacter = (ABaseCharacter*)GetCharacter();
}

void ABasePlayerController::CheckRotation() {
	FVector2D CurrentDirection;
	CurrentDirection.X = BaseCharacter->GetActorForwardVector().X;
	CurrentDirection.Y = BaseCharacter->GetActorForwardVector().Y;
	DesiredDirection = ForwardMovement + RightMovement;
	float DirectionAngle = FMath::RadiansToDegrees(acosf(FVector2D::DotProduct(CurrentDirection, FVector2D(DesiredDirection.GetSafeNormal()))));

	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::SanitizeFloat(DirectionAngle));
	if (DirectionAngle < AngleToRotationThereshold) {
		GetCharacter()->AddMovementInput(DesiredDirection.GetSafeNormal(), DesiredDirection.Size());
		BaseCharacter->bRotatingRight = false;
		BaseCharacter->bRotatingLeft = false;
	}
	else if (DesiredDirection.Size() > 0) {
		float NormalZ = FVector::CrossProduct(FVector(CurrentDirection.X, CurrentDirection.Y, 0), DesiredDirection).Z;
		if (NormalZ > 0) {
			//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Right")));
			if (BaseCharacter->bIsCrouched) {
				GetCharacter()->AddMovementInput(DesiredDirection.GetSafeNormal(), 0.001f);
			}
			else {
				GetCharacter()->AddMovementInput(DesiredDirection.GetSafeNormal(), DesiredDirection.Size());
			}
			BaseCharacter->bRotatingRight = true;
			BaseCharacter->bRotatingLeft = false;
		}
		else {
			//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Left")));
			if (BaseCharacter->bIsCrouched) {
				GetCharacter()->AddMovementInput(DesiredDirection.GetSafeNormal(), 0.001f);
			}
			else {
				GetCharacter()->AddMovementInput(DesiredDirection.GetSafeNormal(), DesiredDirection.Size());
			}
			BaseCharacter->bRotatingLeft = true;
			BaseCharacter->bRotatingRight = false;
		}
	}
	else {
		BaseCharacter->bRotatingRight = false;
		BaseCharacter->bRotatingLeft = false;
	}
}

void ABasePlayerController::MoveForward(float Value)
{
	ForwardMovement = FVector::CrossProduct(BaseCharacter->CurrentCamera->GetRightVector(), FVector::UpVector);
	//ForwardMovement.Z = 0;
	//ForwardMovement.Normalize();
	ForwardMovement *= Value;

	switch (InputState)
	{
	case EInputState::Character:
		if (BaseCharacter->GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Flying) {
			if (BaseCharacter->bBased) {
				BaseCharacter->BaseActor->AddForwardMovementInput(ForwardMovement);
			}
		}
		else if (!BaseCharacter->GetController()->IsMoveInputIgnored()) {
			BaseCharacter->GetCharacterMovement()->Velocity = FVector::UpVector * Value * 100;
		}
		break;
	case EInputState::Aim:
		if (BaseCharacter->bBased) {
			BaseCharacter->BaseActor->AddForwardMovementInput(ForwardMovement);
		}
		break;
	}
}

void ABasePlayerController::MoveRight(float Value)
{
	//FVector CameraForward = BaseCharacter->CurrentCamera->GetForwardVector();
	//CameraForward.Z = 0;
	//CameraForward.Normalize();
	RightMovement = BaseCharacter->CurrentCamera->GetRightVector();
	//RightMovement.Normalize();
	RightMovement *= Value;
	switch (InputState)
	{
	case EInputState::Character:
		if (BaseCharacter->GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Flying) {
			if (!BaseCharacter->bBased) {
				CheckRotation();
			}
			else {
				BaseCharacter->DesiredDirection = ForwardMovement + RightMovement;
				BaseCharacter->BaseActor->AddRightMovementInput(RightMovement);
			}
		}
		break;
	case EInputState::Aim:
		if (!BaseCharacter->bBased) {
			CheckRotation();
		}
		else {
			BaseCharacter->DesiredDirection = ForwardMovement + RightMovement;
			BaseCharacter->BaseActor->AddRightMovementInput(RightMovement);
		}
		break;
	}
}

void ABasePlayerController::PitchCamera(float AxisValue)
{
	// CameraInput.Y = AxisValue * BaseCharacter->CameraSpeed * YAxisDirection;
	// FRotator NewRotation = BaseCharacter->CameraSpringArm->GetComponentRotation();
	// NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + CameraInput.Y, -CameraTopLimit, -CameraBottomLimit);
	// BaseCharacter->CameraSpringArm->SetWorldRotation(NewRotation);
}

void ABasePlayerController::YawCamera(float AxisValue)
{
	// CameraInput.X = AxisValue * BaseCharacter->CameraSpeed;
	// FRotator NewRotation = BaseCharacter->CameraSpringArm->GetComponentRotation();
	// NewRotation.Yaw = NewRotation.Yaw + CameraInput.X;
	// BaseCharacter->CameraSpringArm->SetWorldRotation(NewRotation);
	// if (InputState == EInputState::Aim) {
	// 	FRotator NewCharacterRotation = BaseCharacter->GetActorRotation();
	// 	NewCharacterRotation.Yaw = NewRotation.Yaw; 
	// 	BaseCharacter->SetActorRotation(FMath::Lerp(BaseCharacter->GetActorRotation(), NewCharacterRotation, GetWorld()->DeltaTimeSeconds * 10));
	// }
}

void ABasePlayerController::UpSelection() {
	switch (InputState)
	{
	case EInputState::Terminal:
		//Terminal->Up();
		break;
	case EInputState::UI:
		BaseCharacter->UpSelection();
		break;
	}
}

void ABasePlayerController::DownSelection() {
	switch (InputState)
	{
	case EInputState::Terminal:
		//Terminal->Down();
		break;
	case EInputState::UI:
		BaseCharacter->DownSelection();
		break;
	}
}

void ABasePlayerController::RightSelection() {
	switch (InputState)
	{
	case EInputState::Terminal:
		//Terminal->Right();
		break;
	}
}

void ABasePlayerController::LeftSelection() {
	switch (InputState)
	{
	case EInputState::Terminal:
		//Terminal->Left();
		break;
	}
}

void ABasePlayerController::Accept() {
	switch (InputState)
	{
	case EInputState::Terminal:
		//Terminal->Enter();
		break;
	case EInputState::UI:
		BaseCharacter->AcceptSelection();
		break;
	}
}

void ABasePlayerController::Cancel() {
	switch (InputState)
	{
	case EInputState::Terminal:
		//Terminal->Cancel();
		break;
	}
}

void ABasePlayerController::Interaction() {
	switch (InputState)
	{
	case EInputState::Character:
		BaseCharacter->Interaction();
		BaseCharacter->CheckFrontObject(EInputType::Interaction);
		break;
	}
}

void ABasePlayerController::EndInteraction() {
	switch (InputState)
	{
	case EInputState::Character:
		BaseCharacter->EndInteraction();
		break;
	}
}

void ABasePlayerController::Action() {
	switch (InputState)
	{
	case EInputState::Character:
		BaseCharacter->CheckFrontObject(EInputType::Action);
		break;
	}
}

void ABasePlayerController::AlternativeMovement() {
	switch (InputState)
	{
	case EInputState::Character:
		BaseCharacter->SetAltMovement();
		break;
	}
}

void ABasePlayerController::AlternativeView() {
	switch (InputState)
	{
	case EInputState::Character:
		BaseCharacter->SetFirstPersonCamera(!BaseCharacter->GetCharacterMovement()->IsCrouching());
		break;
	}
}

void ABasePlayerController::Aim() {
	switch (InputState)
	{
	case EInputState::Character:
		BaseCharacter->SetZoomInCamera();
		SetInputState(EInputState::Aim);
		break;
	}
}

void ABasePlayerController::EndAim() {
	switch (InputState)
	{
	case EInputState::Aim:
		BaseCharacter->SetZoomOutCamera();
		SetInputState(EInputState::Character);
		break;
	}
}

void ABasePlayerController::Use() {
	BaseCharacter->Use();
	/*switch (InputState)
	{
	case EInputState::Aim:
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Shot")));
		break;
	}*/
}

void ABasePlayerController::SwitchPause() {
	if (bIsPaused) {
		bIsPaused = false;
		SetPause(false);
		SetInputState(EInputState::Character);
		BaseCharacter->Pause(false);
	}
	else {
		bIsPaused = true;
		SetPause(true);
		SetInputState(EInputState::UI);
		BaseCharacter->Pause(true);
	}
}

void ABasePlayerController::SetInputState(EInputState NewInputState) {
	InputState = NewInputState;
}
