#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Target2D.generated.h"

UCLASS()
class TUI2023_API ATarget2D : public AActor
{
	GENERATED_BODY()
	
public:	
	ATarget2D();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	float t = 0;

	float M = 100.0;
	float G = 9.8;
	float K = 5; //change this later
	float p = 1.2255;
	FVector CurrentVelocity;

	bool bRotate = false;

	FVector BallisticMovement();
	void AerodynamicalRotation(float DeltaTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bMove = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isBallistic = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector StartLocation = FVector(1900, 10, 280);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator StartRotation = FRotator(400, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Velocity = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int RotationSpeed = 720;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector> TargetPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector FromLocation = FVector(0, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator ToRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AActor> MarkerClass;
};
