#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Target2D.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile2D.generated.h"

UCLASS()
class TUI2023_API AProjectile2D : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile2D();

	UFUNCTION(BlueprintCallable, Category = "Custom Initialization")
		void InitializeProjectile2D(bool isBallistic_local, FVector StartLocation_local, FRotator StartRotation_local, FRotator DesiredRotation_local, float Velocity_local, float RotationSpeed_local, float WaitTime_local, TArray<FVector> Path, ATarget2D* TargetRef);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	float G = 9.8f; //9.8f
	FVector CurrentVelocity;

	bool bRotate = false;

	FVector BallisticMovement();
	void AerodynamicalRotation(float DeltaTime);

	float FlightTime(float u, float angle, float y);
	float MaxHeight(float u);
	void ParabolaPoint2D(float u, float x, float y, float& angle1, float& angle2, float& time1, float& time2);

	UFUNCTION(BlueprintCallable)
	bool PredictTrajectory2D(float v1, float v2, float SelfStartRotation, float TargetStartRotation, TArray <float> SelfStartPosition, TArray <float> TargetStartPosition, float& ResultAngle, float Step);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USphereComponent* Sphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bWait = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isBallistic = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector StartLocation = FVector(-2230.f, 0.f, 390.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator StartRotation = FRotator(40.f, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Velocity = 350.f; // 350.f

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RotationSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float WaitTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector> TargetPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator DesiredRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AActor> MarkerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TrajectoryStartMoment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ATarget2D* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CollisionTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> CollisionPosition{0, 0};
};
