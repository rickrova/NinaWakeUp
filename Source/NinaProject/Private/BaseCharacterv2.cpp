// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacterv2.h"
#include "Components/CapsuleComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Engine.h"


ABaseCharacterv2::ABaseCharacterv2()
{
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent((USceneComponent*)GetCapsuleComponent());
	ClimbFixPositionTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ClimbFixPositionTimeline"));
	FixPositionTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("FixPositionTimeline"));
	FirstPersonCameraTransitionTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("FirstPersonCameraTransitionTimeline"));
	ZoomInCameraTransitionTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ZoomInCameraTransitionTimeline"));
	TransparentTransitionTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TransparentTransitionTimeline"));
	CurrentCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DefaultCamera"));
	Inventory = CreateDefaultSubobject<UInventory>(TEXT("Inventory"));

	RayOffset = 25;
	RayLength = 100;
	FrontRayLength = 50;
	VerticalRayOffset = 110;
	VerticalRayLength = 120;
	ClimbOffsetForwardThereshold = 65;
	PushRotationSpeed = 3.0f;
	InitialRotationRate = GetCharacterMovement()->RotationRate;
	GrabWalkSpeed = 200;
	AimWalkSpeed = 100;
	ZoomInCameraOffset = FVector(0, 60, 0);
	MinTransparency = 0.25;
	JetpackForce = 120;
	JetpackEnergyRecoverSpeed = 1.0f;
	JetpackEnergyDecreaseSpeed = 4.0f;
	JetpackMaxEnergy = 1.0f;
	JetpackRotationSpeed = 200;
	JetpackMaxRoll = 90;
	JetpackBoostPower = 2;
	JetpackSlowEnergyRecover = 0.5f;

	LifeUnits = 3;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
}

void ABaseCharacterv2::BeginPlay()
{
	Super::BeginPlay();
	AnimInstance = (UHumanAnimInstance*)GetMesh()->GetAnimInstance();
	GeneralMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), GeneralMaterial);
	GetMesh()->SetMaterial(0, GeneralMaterial);
	HairMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(1), HairMaterial);
	GetMesh()->SetMaterial(1, HairMaterial);
	InitialWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

	ClimbFixPositionCallback.BindUFunction(this, FName{ TEXT("ClimbFixPositionTimelineCallback") });
	ClimbFixPositionTimeline->AddInterpFloat(EaseInOutCurve, ClimbFixPositionCallback, FName("ClimbPosition"));
	FixPositionCallback.BindUFunction(this, FName{ TEXT("FixPositionTimelineCallback") });
	FixPositionTimeline->AddInterpFloat(EaseInOutCurve, FixPositionCallback, FName("Position"));
	FirstPersonCameraTransitionCallback.BindUFunction(this, FName{ TEXT("FirstPersonCameraTimelineCallback") });
	FirstPersonCameraTransitionTimeline->AddInterpFloat(EaseInOutCurve, FirstPersonCameraTransitionCallback, FName("FirstPersonCameraPosition"));
	ZoomInCameraTransitionCallback.BindUFunction(this, FName{ TEXT("ZoomInCameraTimelineCallback") });
	ZoomInCameraTransitionTimeline->AddInterpFloat(EaseInOutCurve, ZoomInCameraTransitionCallback, FName("ZoomInCameraPosition"));
	TransparentTransitionCallback.BindUFunction(this, FName{ TEXT("TransparentTimelineCallback") });
	TransparentTransitionTimeline->AddInterpFloat(EaseInOutCurve, TransparentTransitionCallback, FName("TransparentPosition"));

	GetController()->SetIgnoreMoveInput(true);
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("NINA START")));
}

void ABaseCharacterv2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult* HitResult = new FHitResult();
	FVector StartTrace = GetActorLocation()
		+ GetActorForwardVector().GetSafeNormal() * FrontRayOffset;
	FVector FrontEndTrace = StartTrace + GetActorForwardVector().GetSafeNormal() * FrontRayLength;
	FCollisionQueryParams TraceParams = FCollisionQueryParams();

	TraceParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, FrontEndTrace, ECC_Visibility, TraceParams)) {
		if (HitResult->GetActor()->ActorHasTag(FName("Climbable")) && !bUI) {
			bUI = true;
			ShowUI(0);
		}
		else if (HitResult->GetActor()->ActorHasTag(FName("Jump")) && !bUI) {
			bUI = true;
			ShowUI(1);
		}
		else if (HitResult->GetActor()->ActorHasTag(FName("Crawl")) && !bUI) {
			bUI = true;
			ShowUI(2);
		}
		else if (HitResult->GetActor()->ActorHasTag(FName("Orb")) && !bUI) {
			bUI = true;
			ShowUI(3);
		}
		else if (HitResult->GetActor()->ActorHasTag(FName("Box")) && !bUI) {
			bUI = true;
			ShowUI(4);
		}
		else if (HitResult->GetActor()->ActorHasTag(FName("Terminal")) && !bUI) {
			bUI = true;
			ShowUI(5);
		}
		/*else if (bUI) {
			bUI = false;
			HideUI();
		}*/
	}
	else if (bUI) {
		bUI = false;
		HideUI();
	}

	if (bZoomIn && bAimGadget) {
		GetCurvePosition();
	}

	if (bInvincible) {
		BlinkMaterial();
	}
}

void ABaseCharacterv2::CheckFrontObject(EInputType2 InputType) {
	if (!bClimbing) {
		//PreInteraction();
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("CHECK INTERACTION")));
		FHitResult* HitResult = new FHitResult();
		FVector StartTrace = GetActorLocation()
			+ GetActorForwardVector().GetSafeNormal() * FrontRayOffset;
		FVector FrontEndTrace = StartTrace + GetActorForwardVector().GetSafeNormal() * FrontRayLength;
		FCollisionQueryParams TraceParams = FCollisionQueryParams();
		TraceParams.AddIgnoredActor(this);
		//DrawDebugLine(GetWorld(), StartTrace, FrontEndTrace, FColor(0, 255, 0), false);
		//FCollisionShape CollisionShape = FCollisionShape::MakeBox(FVector(1, 1, 70));

		//DrawDebugBox(GetWorld(), StartTrace, CollisionShape.GetBox(), FColor(0, 255, 0), false, 0.1f);
		//DrawDebugBox(GetWorld(), FrontEndTrace, CollisionShape.GetBox(), FColor(0, 255, 0), false, 0.1f);

		if (GetWorld()->LineTraceSingleByChannel(*HitResult, StartTrace, FrontEndTrace, ECC_Visibility, TraceParams)) {
			//SweepSingleByChannel(*HitResult, StartTrace, FrontEndTrace, FQuat::Identity, ECC_Visibility, CollisionShape, TraceParams)){
			//LineTraceSingleByChannel(*HitResult, StartTrace, FrontEndTrace, ECC_Visibility, TraceParams)) {
			//ExposedHit = *HitResult;
			/*GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, HitResult->GetActor()->GetName());
			DrawDebugLine(
				GetWorld(),
				StartTrace,
				FrontEndTrace,
				FColor(255, 0, 0),
				false, -1, 0,
				50.0f
			);*/
			switch (InputType) {
			case EInputType2::Action:
				//check object action capabilities before call action and there is no need to bridge with blueprints
				if (HitResult->GetActor()->ActorHasTag(FName("Action"))) {
					//Action();

					FHitResult* VerticalHitResult = new FHitResult();
					FVector VerticalStartTrace = HitResult->ImpactPoint + FVector::UpVector * VerticalRayOffset - HitResult->ImpactNormal;
					FVector VerticalEndTrace = VerticalStartTrace - FVector::UpVector * VerticalRayLength;
					GetWorld()->LineTraceSingleByChannel(*VerticalHitResult, VerticalStartTrace, VerticalEndTrace, ECC_Visibility, TraceParams);
					if (VerticalHitResult->Distance > 50) {
						HitResult->ImpactPoint.Z = VerticalHitResult->ImpactPoint.Z - 35;
						Climb(HitResult);
					}
				}
				break;
			case EInputType2::Interaction:
				StartInteraction(HitResult->GetActor(), *HitResult);
				break;
			}
		}
		else {
			switch (InputType) {
			case EInputType2::Action:
				if (bWakeUp) {
					Jump();
				}
				else {//Remove wake up check as soon as posible
					bWakeUp = true;
					NinaWakeUp();
				}
				break;
			}
		}
	}
}

/*void ABaseCharacterv2::ClearInteractable() {
	EndInteraction();
}*/

void ABaseCharacterv2::SetAltMovement() {
	if (GetCharacterMovement()->IsCrouching()) {
		UnCrouch();
		FVector Euler = FVector(0, 0, 540);
		FRotator NewRotationRate = FRotator::MakeFromEuler(Euler);
		GetCharacterMovement()->RotationRate = NewRotationRate;
	}
	else {
		Crouch();
		FVector Euler = FVector(0, 0, 200);
		FRotator NewRotationRate = FRotator::MakeFromEuler(Euler);
		GetCharacterMovement()->RotationRate = NewRotationRate;
	}
}

/*void ABaseCharacterv2::Action() {
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Action")));
}*/

void ABaseCharacterv2::OnInteraction(FHitResult HitResult) {
	AActor* InteractiveActor = HitResult.GetActor();
	AttachToComponent(InteractiveActor->GetRootComponent(),
		FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true), NAME_None);
	/*CameraSpringArm->AttachToComponent(InteractiveActor->GetRootComponent(),
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false),
		NAME_None);*/
	FVector BoxOrigin;
	FVector BoxExtent;
	InteractiveActor->GetActorBounds(true, BoxOrigin, BoxExtent);
	bBased = true;
	BaseActor = (ABaseMovableActor*)InteractiveActor;
	FixPosition(&HitResult, false);
}

void ABaseCharacterv2::OnInteractionEnd() {
	FixPositionTimeline->Stop();
	DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false));
	/*CameraSpringArm->AttachToComponent(GetRootComponent(),
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false),
		NAME_None);*/
		//DetachRootComponentFromParent(true);
		/*AttachToComponent(Interactive->GetRootComponent(),
			FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true), NAME_None);*/
	bBased = false;
}

void ABaseCharacterv2::OnAction() {
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Action")));
}

void ABaseCharacterv2::Climb(FHitResult* HitResult) {
	bClimbing = true;
	bPendingJump = false;
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
	GetCharacterMovement()->GravityScale = 0;
	FixPosition(HitResult, true);
	AnimInstance->bClimb = true;
	GetController()->SetIgnoreMoveInput(true);
}

void ABaseCharacterv2::ClimbFixPositionTimelineCallback() {
	FVector CurrentLocation = FMath::Lerp(InitialLocation, DesiredLocation, EaseInOutCurve->GetFloatValue(ClimbFixPositionTimeline->GetPlaybackPosition()));
	FRotator CurrentRotation = FMath::Lerp(InitialRotation, DesiredRotation, EaseInOutCurve->GetFloatValue(ClimbFixPositionTimeline->GetPlaybackPosition()));
	SetActorRotation(CurrentRotation);
	SetActorLocation(CurrentLocation);
	if (ClimbFixPositionTimeline->GetPlaybackPosition() >= ClimbFixPositionTimeline->GetTimelineLength()) {
		//Do something on finish
	}
}

void ABaseCharacterv2::FixPositionTimelineCallback() {
	FVector CurrentLocation = FMath::Lerp(InitialLocation, DesiredLocation, EaseInOutCurve->GetFloatValue(FixPositionTimeline->GetPlaybackPosition()));
	FRotator CurrentRotation = FMath::Lerp(InitialRotation, DesiredRotation, EaseInOutCurve->GetFloatValue(FixPositionTimeline->GetPlaybackPosition()));
	SetActorRotation(CurrentRotation);
	SetActorLocation(CurrentLocation);
	if (FixPositionTimeline->GetPlaybackPosition() >= FixPositionTimeline->GetTimelineLength()) {
		//Do something on finish
		//Controller->SetIgnoreMoveInput(false);
		OnFixPositionEnd();
	}
}

void ABaseCharacterv2::OnClimbEnd() {
	//CameraSpringArm->SetRelativeLocation(InitialSpringArmOffset);
	//GetController()->SetIgnoreMoveInput(false);
	GetCharacterMovement()->RotationRate = InitialRotationRate;
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
	GetCharacterMovement()->GravityScale = 1;
	ClimbFixPositionTimeline->Stop();
	SetActorLocation(DesiredLocation + ClimbOffset);
}

void ABaseCharacterv2::OnGrab(FHitResult HitResult) {
	//Controller->SetIgnoreMoveInput(true);
	bGrab = true;
	GetCharacterMovement()->RotationRate = InitialRotationRate * 0.5f;
	GetCharacterMovement()->MaxWalkSpeed = GrabWalkSpeed;
	//FixPosition(&HitResult, false);
}

void ABaseCharacterv2::OnGrabEnd() {
	bGrab = false;
	GetCharacterMovement()->RotationRate = InitialRotationRate * 2;
	GetCharacterMovement()->MaxWalkSpeed = InitialWalkSpeed;
}

void ABaseCharacterv2::FixPosition(FHitResult* HitResult, bool bClimb) {
	DesiredLocation = HitResult->ImpactPoint + HitResult->ImpactNormal * 20;
	DesiredRotation = (-HitResult->ImpactNormal).Rotation();
	InitialLocation = GetActorLocation();
	InitialRotation = GetActorRotation();
	FOnTimelineFloat Callback{};
	if (bClimb) {
		ClimbOffset = FVector::UpVector * 100 - HitResult->ImpactNormal * ClimbOffsetForwardThereshold;
		ClimbFixPositionTimeline->PlayFromStart();
	}
	else {
		//ClimbOffset = FVector::UpVector * 0 - HitResult->ImpactNormal * ClimbOffsetForwardThereshold;
		FixPositionTimeline->PlayFromStart();
	}
}

void ABaseCharacterv2::SetFirstPersonCamera(bool InValue) {
	if (GetCharacterMovement()->IsCrouching()) {
		bIsFirstPerson = InValue;
		if (bIsFirstPerson) {
			//InitialSpringArmLength = CameraSpringArm->TargetArmLength;
		}
		else {
		}
		/*FOnTimelineFloat Callback{};
		Callback.BindUFunction(this, FName{ TEXT("FirstPersonCameraTimelineCallback") });*/
		FirstPersonCameraTransitionTimeline->PlayFromStart();
	}
}

void ABaseCharacterv2::FirstPersonCameraTimelineCallback() {
	if (bIsFirstPerson) {
		GeneralMaterial->SetScalarParameterValue(FName("Alpha"), 1 - EaseInOutCurve->GetFloatValue(FirstPersonCameraTransitionTimeline->GetPlaybackPosition()));
		HairMaterial->SetScalarParameterValue(FName("Alpha"), 1 - EaseInOutCurve->GetFloatValue(FirstPersonCameraTransitionTimeline->GetPlaybackPosition()));
	}
	else {
		GeneralMaterial->SetScalarParameterValue(FName("Alpha"), EaseInOutCurve->GetFloatValue(FirstPersonCameraTransitionTimeline->GetPlaybackPosition()));
		HairMaterial->SetScalarParameterValue(FName("Alpha"), EaseInOutCurve->GetFloatValue(FirstPersonCameraTransitionTimeline->GetPlaybackPosition()));
	}
}

void ABaseCharacterv2::ZoomInCameraTimelineCallback() {
	if (bZoomIn) {/*
		CameraSpringArm->TargetArmLength = FMath::Lerp(InitialSpringArmLength, ZoomInCameraSpringArmLength,
			EaseInOutCurve->GetFloatValue(ZoomInCameraTransitionTimeline->GetPlaybackPosition()));
		CameraSpringArm->SetRelativeLocation(FMath::Lerp(InitialSpringArmOffset, ZoomInCameraOffset,
			EaseInOutCurve->GetFloatValue(ZoomInCameraTransitionTimeline->GetPlaybackPosition())));*/
	}
	else {/*
		CameraSpringArm->TargetArmLength = FMath::Lerp(ZoomInCameraSpringArmLength, InitialSpringArmLength,
			EaseInOutCurve->GetFloatValue(ZoomInCameraTransitionTimeline->GetPlaybackPosition()));
		CameraSpringArm->SetRelativeLocation(FMath::Lerp(ZoomInCameraOffset, InitialSpringArmOffset,
			EaseInOutCurve->GetFloatValue(ZoomInCameraTransitionTimeline->GetPlaybackPosition())));*/
	}
}

void ABaseCharacterv2::SetZoomInCamera() {
	bZoomIn = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;
	//CameraSpringArm->SetRelativeLocation(ZoomInCameraOffset);
	//InitialSpringArmLength = CameraSpringArm->TargetArmLength;
	ZoomInCameraTransitionTimeline->PlayFromStart();
	Zoom(true);
}

void ABaseCharacterv2::SetZoomOutCamera() {
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
	bZoomIn = false;
	bAimGadget = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = InitialWalkSpeed;
	//CameraSpringArm->SetRelativeLocation(InitialSpringArmOffset);
	ZoomInCameraTransitionTimeline->PlayFromStart();
	Zoom(false);
}

void ABaseCharacterv2::Jump() {
	//Check child class for details
}

void ABaseCharacterv2::FloatUp(float DeltaTime) {
	//Check child class for details
}

void ABaseCharacterv2::StopJump() {
	//Check child class for details
}

void ABaseCharacterv2::JetpackBoost() {
	//Check Child class for details
}

void ABaseCharacterv2::HideNina(bool InValue) {
	bIsHide = InValue;
	TransparentTransitionTimeline->PlayFromStart();
}

void ABaseCharacterv2::TransparentTimelineCallback() {
	if (bIsHide) {
		GeneralMaterial->SetScalarParameterValue(FName("Alpha"),
			FMath::Clamp(1 - EaseInOutCurve->GetFloatValue(TransparentTransitionTimeline->GetPlaybackPosition()), MinTransparency, 1.0f));
		HairMaterial->SetScalarParameterValue(FName("Alpha"),
			FMath::Clamp(1 - EaseInOutCurve->GetFloatValue(TransparentTransitionTimeline->GetPlaybackPosition()), MinTransparency, 1.0f));
	}
	else {
		GeneralMaterial->SetScalarParameterValue(FName("Alpha"),
			FMath::Clamp(EaseInOutCurve->GetFloatValue(TransparentTransitionTimeline->GetPlaybackPosition()), MinTransparency, 1.0f));
		HairMaterial->SetScalarParameterValue(FName("Alpha"),
			FMath::Clamp(EaseInOutCurve->GetFloatValue(TransparentTransitionTimeline->GetPlaybackPosition()), MinTransparency, 1.0f));
	}
}

void ABaseCharacterv2::GetCurvePosition() {
	/*FVector InitialOffset = Camera->GetComponentLocation();//GetActorLocation() + FVector::ForwardVector * 100;
	FVector InitialCurveVeocity = Camera->GetForwardVector(); FVector::UpVector + GetActorForwardVector();
	InitialCurveVeocity.Normalize();
	InitialCurveVeocity *= 100;
	float PointsDistance = 0.25f;
	int NumberOfPoints = 50;
	for (int i = 0; i < NumberOfPoints; i++)
	{
		FVector CurrentPosition = InitialCurveVeocity * i * PointsDistance - FVector::UpVector * 9.8f * 0.5f * FMath::Square(i * PointsDistance);
		FVector NextPosition = InitialCurveVeocity * (i + 1) * PointsDistance - FVector::UpVector * 9.8f * 0.5f * FMath::Square((i + 1) * PointsDistance);
		//DrawDebugPoint(GetWorld(), InitialOffset + CurrentPosition, 5, FColor::Green, false, -1, 0);

		FHitResult HitResult;
		FCollisionShape CollisionShape;
		CollisionShape.ShapeType = ECollisionShape::Sphere;

		if (GetWorld()->SweepSingleByChannel(HitResult, InitialOffset + CurrentPosition,
			InitialOffset + NextPosition, FQuat::Identity, ECollisionChannel::ECC_Visibility, CollisionShape)) {
			DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10, FColor::Red, false, -1, 0);
			break;
		}

	}*/

	FHitResult HitResult;
	FCollisionShape CollisionShape;
	CollisionShape.ShapeType = ECollisionShape::Sphere;

	/*if (GetWorld()->SweepSingleByChannel(HitResult, Camera->GetComponentLocation(),
		Camera->GetComponentLocation() + Camera->GetForwardVector() * 5000, FQuat::Identity, ECollisionChannel::ECC_Visibility, CollisionShape)) {
		DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10, FColor::Red, false, -1, 0);
	}*/

}

void ABaseCharacterv2::BlinkMaterial() {
	if (!bIsHide && bInvincible) {
		GeneralMaterial->SetScalarParameterValue(FName("Alpha"), 0);
		HairMaterial->SetScalarParameterValue(FName("Alpha"), 0);
		bIsHide = true;
	}
	else {
		GeneralMaterial->SetScalarParameterValue(FName("Alpha"), 1);
		HairMaterial->SetScalarParameterValue(FName("Alpha"), 1);
		bIsHide = false;
	}
}

void ABaseCharacterv2::SetCamera(UCameraComponent* inCamera) {
	CameraTransitionYaw = GetActorRotation().Yaw;
	if (!CurrentCamera) {
		CurrentCamera = inCamera;
	}
	else {
		PendingCamera = inCamera;
	}
}