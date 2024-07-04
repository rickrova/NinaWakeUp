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
	InputComponent->BindAxis("PanningHorizontal", this, &ABasePlayerController::PanningHor);
	InputComponent->BindAxis("PanningVertical", this, &ABasePlayerController::PanningVert);
	InputComponent->BindAxis("ZoomIn", this, &ABasePlayerController::ZoomInItem);
	InputComponent->BindAxis("ZoomOut", this, &ABasePlayerController::ZoomOutItem);

	InputComponent->BindAction("Interact", IE_Pressed, this, &ABasePlayerController::Interaction);
	InputComponent->BindAction("Interact", IE_Released, this, &ABasePlayerController::EndInteraction);
	InputComponent->BindAction("Action", IE_Pressed, this, &ABasePlayerController::Action);
	InputComponent->BindAction("AimThrow", IE_Pressed, this, &ABasePlayerController::AimCode);
	InputComponent->BindAction("AimThrow", IE_Released, this, &ABasePlayerController::ThrowCode);
	InputComponent->BindAction("AlternativeMovement", IE_Pressed, this, &ABasePlayerController::AlternativeMovement);
	InputComponent->BindAction("AlternativeView", IE_Pressed, this, &ABasePlayerController::AlternativeView);
	InputComponent->BindAction("NextPage", IE_Pressed, this, &ABasePlayerController::NextPage);
	InputComponent->BindAction("PrevPage", IE_Pressed, this, &ABasePlayerController::PrevPage);
	InputComponent->BindAction("Use", IE_Pressed, this, &ABasePlayerController::Use);
	InputComponent->BindAction("AltTriggers", IE_Pressed, this, &ABasePlayerController::AltTriggers);
	InputComponent->BindAction("Pause", IE_Pressed, this, &ABasePlayerController::SwitchPause).bExecuteWhenPaused = true;
	InputComponent->BindAction("Menu", IE_Pressed, this, &ABasePlayerController::SwitchMenu).bExecuteWhenPaused = false;

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
	HUD = Cast<ANWUHUD>(GetHUD());
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

	HUD->MoveCursorVertical(-Value);
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
		if (!BaseCharacter->bBased) {
			float deltaTime = GetWorld()->DeltaTimeSeconds;
			float clampStrenght = 5.f * FMath::Abs(Value);
			BaseCharacter->ThrowForce += Value * deltaTime * 0.5f;
			//BaseCharacter->ThrowForce = FMath::Clamp(BaseCharacter->ThrowForce, 0.1f, 0.75f);
			if (BaseCharacter->ThrowForce < 0.15f) {
				BaseCharacter->ThrowForce = FMath::Lerp(BaseCharacter->ThrowForce, 0.15f, deltaTime * clampStrenght);
			}
			else if (BaseCharacter->ThrowForce > 0.6f) {
				BaseCharacter->ThrowForce = FMath::Lerp(BaseCharacter->ThrowForce, 0.6f, deltaTime * clampStrenght);
			}
		}
		break;
	case EInputState::UI:
		HUD->Vertical(-Value);
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

	HUD->MoveCursorHorizontal(Value);
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
			FRotator deltaRotator = FRotator(0, Value, 0);
			GetCharacter()->AddActorLocalRotation(deltaRotator);
		}
		break;
	case EInputState::UI:
		HUD->Horizontal(-Value);
		break;
	}
}

void ABasePlayerController::PanningHor(float AxisValue)
{
	switch (InputState)
	{
	case EInputState::UI:
			Panning.X = AxisValue;
		break;
	}
}
void ABasePlayerController::PanningVert(float AxisValue)
{
	switch (InputState)
	{
	case EInputState::UI:
			Panning.Y = AxisValue;
			HUD->PanItem(Panning);
		break;
	}
}

void ABasePlayerController::ZoomInItem(float AxisValue)
{
	switch (InputState)
	{
		case EInputState::UI:
			if (bAltZoomFunction) {
				RollDelta = AxisValue;
			}
			else {
				ZoomDelta = AxisValue;
			}
			break;
	}
}
void ABasePlayerController::ZoomOutItem(float AxisValue)
{
	switch (InputState)
	{
	case EInputState::UI:
		if (bAltZoomFunction) {
			RollDelta += AxisValue;
			HUD->RollItem(RollDelta);
		}
		else {
			ZoomDelta += AxisValue;
			HUD->ZoomItem(ZoomDelta);
		}
		break;
	}
}

void ABasePlayerController::AltTriggers()
{
	switch (InputState)
	{
	case EInputState::UI:
		bAltZoomFunction = !bAltZoomFunction;
		break;
	}
}

void ABasePlayerController::UpSelection() {
	switch (InputState)
	{
	case EInputState::Terminal:
		//Terminal->Up();
		break;
	case EInputState::UI:
		HUD->Up();
		break;
	case EInputState::Character:
		UpOrder();
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
		HUD->Down();
		break;
	case EInputState::Character:
		DownOrder();
		break;
	}
}

void ABasePlayerController::RightSelection() {
	switch (InputState)
	{
	case EInputState::Terminal:
		//Terminal->Right();
		break;
	case EInputState::UI:
		HUD->Right();
		break;
	case EInputState::Character:
		RightOrder();
		break;
	}
}

void ABasePlayerController::LeftSelection() {
	switch (InputState)
	{
	case EInputState::Terminal:
		//Terminal->Left();
		break;
	case EInputState::UI:
		HUD->Left();
		break;
	case EInputState::Character:
		LeftOrder();
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
		HUD->Accept();
		break;
	}
}

void ABasePlayerController::Cancel() {
	switch (InputState)
	{
	case EInputState::Terminal:
		//Terminal->Cancel();
		break;
	case EInputState::UI:
		HUD->Cancel();
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

void ABasePlayerController::AimCode() {
	BaseCharacter->ThrowForce = 0.5f;
	InputState = EInputState::Aim;
	Aim();
}

void ABasePlayerController::ThrowCode() {
	InputState = EInputState::Character;
	Throw();
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

void ABasePlayerController::NextPage() {
	switch (InputState)
	{
	case EInputState::UI:
		HUD->NextPage();
		break;
	}
}

void ABasePlayerController::PrevPage() {
	switch (InputState)
	{
	case EInputState::UI:
		HUD->PrevPage();
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
		HUD->SwitchPanelVisibility(9, 6);
		SetInputState(EInputState::Character);
		bIsPaused = false;
		SetPause(false);
		BaseCharacter->Pause(false);
	}
	else {
		if (InputState == EInputState::UI) {
			HUD->SwitchPanelVisibility(6, 5);
			HUD->SwitchPanelVisibility(6, 10);
			HUD->SwitchPanelVisibility(6, 11);
			HUD->CloseInspectMesh();
		}
		else {
			HUD->SwitchPanelVisibility(6, 9);
		}
		SetInputState(EInputState::UI);
		bIsPaused = true;
		SetPause(true);
		BaseCharacter->Pause(true);
	}
}

void ABasePlayerController::SwitchMenu() {
	if (InputState == EInputState::UI) {
		if (bIsPaused) {
			SwitchPause();
		}
		else {
			HUD->SwitchPanelVisibility(9, 5);
			HUD->SwitchPanelVisibility(9, 10);
			HUD->SwitchPanelVisibility(9, 11);
			HUD->CloseInspectMesh();
			SetInputState(EInputState::Character);
		}
	}
	else {
		HUD->SwitchPanelVisibility(5, 9);
		SetInputState(EInputState::UI);
	}
}

void ABasePlayerController::SetInputState(EInputState NewInputState) {
	InputState = NewInputState;
}
