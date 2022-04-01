// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseMovableActor.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"


// Sets default values
ABaseMovableActor::ABaseMovableActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent((USceneComponent*)VisibleComponent);
	Forward = CreateDefaultSubobject<UArrowComponent>(TEXT("Forward"));
	Forward->SetupAttachment(GetRootComponent());
	Handle = CreateDefaultSubobject<USceneComponent>(TEXT("Handle"));
	Handle->SetupAttachment(Forward);

	AngularVelocityThereshold = 20;
	LinearVelocityThereshold = 40;
	ZRotationSpeed = 2.5f;
	ForceMultiplier = 250000;
	TraceLength = 51;
    StartTraceOffset = FVector(0,0,1); 

	CurrentDirection = 1;
	MaxReverseAngle = 120;
}

// Called when the game starts or when spawned
void ABaseMovableActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
	//VisibleComponent->SetSimulatePhysics(false);
}

// Called every frame
void ABaseMovableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetVelocity().Size() == 0 && !bInteracting) {
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Box Stop")));
		SetActorTickEnabled(false);
		VisibleComponent->SetSimulatePhysics(false);
		return;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Tick")));
	FVector Movement = (RightAxis + FrontAxis);
	Movement.Z = 0;
	float Force = Movement.Size() * ForceMultiplier;
	//Movement.Normalize();
		
	if (Movement.Size() > 0) {
		if (bCanSwitchDirection) {
			bCanSwitchDirection = false;
			Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Movement.GetSafeNormal(), Forward->GetForwardVector())));
			if (Angle > MaxReverseAngle) {
				CurrentDirection = -1;
				bReverse = true;
			}
			else {
				CurrentDirection = 1;
				bReverse = false;
			}
		}

		Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Movement.GetSafeNormal(),
			Forward->GetForwardVector() * CurrentDirection))) * Movement.Size();
		FVector AngleNormal = FVector::CrossProduct(Movement.GetSafeNormal(), Forward->GetForwardVector());
		AngleDirection = FMath::Abs(AngleNormal.Z) / AngleNormal.Z;

		if (CheckFloor(Forward->GetComponentLocation() + Forward->GetForwardVector() * StartTraceOffset.X
			+ FVector::UpVector * StartTraceOffset.Z, TraceLength)){
			FVector NewHandleLocation = FVector::ZeroVector;
			NewHandleLocation.Y = Angle * AngleDirection; // *Direction * AngleDirection;
			FVector NewLinearVelocity = VisibleComponent->GetPhysicsLinearVelocity();
			NewLinearVelocity.X = FMath::Clamp(NewLinearVelocity.X, -LinearVelocityThereshold, LinearVelocityThereshold);
			NewLinearVelocity.Y = FMath::Clamp(NewLinearVelocity.Y, -LinearVelocityThereshold, LinearVelocityThereshold);
			FVector NewAngularVelocity = VisibleComponent->GetPhysicsAngularVelocityInDegrees();
			NewAngularVelocity.Z = FMath::Clamp(NewAngularVelocity.Z, -AngularVelocityThereshold, AngularVelocityThereshold);

			Handle->SetRelativeLocation(NewHandleLocation, false, nullptr, ETeleportType::None);
			ZDeltaRotation = NewHandleLocation.Y;

			/*FString DirectionString = FString::SanitizeFloat(Direction);
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, DirectionString);*/

			VisibleComponent->AddForceAtLocation(Forward->GetForwardVector() * Force * CurrentDirection, Handle->GetComponentLocation(), NAME_None);
			VisibleComponent->SetPhysicsLinearVelocity(NewLinearVelocity, false, NAME_None);
			VisibleComponent->SetPhysicsAngularVelocityInDegrees(NewAngularVelocity, false, NAME_None);
			//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::SanitizeFloat(Movement.Size()));
			/*DrawDebugPoint(GetWorld(),
				Handle->GetComponentLocation(),
				10.0f,
				FColor(255, 0, 0),
				false,
				-1,
				SDPG_MAX);*/
		}
		else {
			//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Ray not collide")));
			VisibleComponent->SetCenterOfMass(FVector::ZeroVector, NAME_None);
			//OnInteractionEnd();
			//SetActorTickEnabled(false);
			bInterruptInteraction = true;
		}
	}
	else if(!bCanSwitchDirection) {
		bCanSwitchDirection = true;
		CurrentDirection = 1;
	}

	/*DrawDebugPoint(GetWorld(),
		VisibleComponent->GetCenterOfMass(),
		5.0f,
		FColor(0, 255, 0),
		false,
		-1,
		SDPG_MAX);
	DrawDebugPoint(GetWorld(),
		Forward->GetComponentLocation(),
		10.0f,
		FColor(0, 0, 255),
		false,
		-1,
		SDPG_MAX);*/
}

void ABaseMovableActor::AddRightMovementInput(FVector Axis) {
	RightAxis = Axis;
	//RightForce = Value;
}

void ABaseMovableActor::AddForwardMovementInput(FVector Axis) {
	FrontAxis = Axis;
	//FrontForce = Value;
}

void ABaseMovableActor::OnInteraction(FHitResult InHit) {
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Interaction")));
	bInteracting = true;
	Hit = InHit;
	FVector Origin = VisibleComponent->Bounds.Origin;
	FVector Offset = FVector(0, 0, -50);
	if (CheckFloor(Origin, 500)) {
		Offset.Z = FloorHit->ImpactPoint.Z - Origin.Z;
	}
	bReverse = false;
	SetActorTickEnabled(true);
	Forward->SetWorldLocation(Origin + Offset, false, nullptr, ETeleportType::None);
	Forward->SetWorldRotation((-Hit.Normal).Rotation(), false, nullptr, ETeleportType::None);
	FVector CenterOfMass = Forward->GetRelativeTransform().GetLocation() + Offset;
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, Offset.ToString());
	//FVector CenterOfMass = VisibleComponent->GetComponentTransform().InverseTransformPosition(Origin + Offset);
	VisibleComponent->SetCenterOfMass(CenterOfMass);
	VisibleComponent->SetSimulatePhysics(true);
}

void ABaseMovableActor::OnInteractionEnd() {
	RightAxis = FVector::ZeroVector;
	FrontAxis = FVector::ZeroVector;
	bInteracting = false;
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Interaction End")));
	//SetActorTickEnabled(false);
	bInterruptInteraction = false;
	//VisibleComponent->SetSimulatePhysics(false);
}

void ABaseMovableActor::SnapToMagnet(FVector NewLocation) {
	VisibleComponent->SetSimulatePhysics(false);
	bInterruptInteraction = true;
	NewLocation.Z = GetActorLocation().Z;
	SetActorLocation(NewLocation);
}

bool ABaseMovableActor::CheckFloor(FVector InOrigin, float InLength) {
	//FVector StartTrace = Forward->GetComponentLocation() + StartTraceOffset;
	FVector EndTrace = InOrigin - FVector::UpVector * InLength;
	FloorHit = new FHitResult();

	/*DrawDebugLine(
		GetWorld(),
		InOrigin,
		EndTrace,
		FColor(255, 0, 0),
		false, -1, 0,
		50.0f
	);*/

	return GetWorld()->LineTraceSingleByChannel(*FloorHit, InOrigin, EndTrace, ECC_Visibility,
		FCollisionQueryParams::DefaultQueryParam);
}
