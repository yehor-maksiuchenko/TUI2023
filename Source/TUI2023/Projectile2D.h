#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile2D.generated.h"

UCLASS()
class TUI2023_API AProjectile2D : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile2D();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	float M = 100.f; // 100.f
	float G = 9.8f; //9.8f
	float K = 5.f; //5.f
	float p = 1.2255f; // 1.2255f
	float pi = 3.14159265359; 
	FVector CurrentVelocity;

	bool bRotate = false;

	FVector BallisticMovement();
	void AerodynamicalRotation(float DeltaTime);

	float FlightTime(float u, float angle, float y);
	void ParabolaPoint(float u, float x, float y, float& angle1, float& angle2, float& time1, float& time2);
	void ParabolaParabola(float u1, float u2, float angle2, float x_0, float y_0, float& result_angle, float Step, float& CollisionTime, float& WaitTime, TArray<float>& CollisionPosition);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USphereComponent* Sphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bMove = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isBallistic = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector StartLocation = FVector(1900.f, 0.f, 280.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator StartRotation = FRotator(40.f, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Velocity = 200.f; // 200.f

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int RotationSpeed = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector> TargetPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector FromLocation = FVector(0, 0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator ToRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AActor> MarkerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TrajectoryStartMoment;
};
