// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Item.generated.h"

USTRUCT(BlueprintType)
struct FItemMeta {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* Image;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UMeshComponent> MeshArchetype;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMesh* StaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMesh* SkeletalMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TAssetPtr<UWorld> UnlockableLevel;
};

UCLASS()
class NINAPROJECT_API AItem : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere)
	USceneComponent* Root;
	UPROPERTY(EditAnywhere)
	USphereComponent* Collision;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* VisibleComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FItemMeta MetaData;

	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UFUNCTION()
	//void OnOverlapBegin(class UPrimitiveComponent* thisComp,
	//class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	//int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult);
};
