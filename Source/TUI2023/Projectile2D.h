#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Target2D.h"
#include "TUI2023.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile2D.generated.h"

UCLASS()
class TUI2023_API AProjectile2D : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile2D();

	UFUNCTION(BlueprintCallable, Category = "Custom Initialization", meta = (g = 9.8, SimulationSpeed = 1.0))
		void InitializeProjectile2D(FProjectileParams ProjectileParams, ATarget2D* TargetRef, float g, float SimulationSpeed);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	FVector CurrentVelocity;

	bool bRotate = false;

	FVector BallisticMovement();
	void AerodynamicalRotation(float DeltaTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* Sphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bWait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isBallistic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator StartRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaitTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> Path = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator DesiredRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> MarkerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATarget2D* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float G = 9.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SimulationSpeedMultiplier = 1.f;
};
