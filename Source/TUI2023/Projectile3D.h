#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Target3D.h"
#include "TUI2023.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile3D.generated.h"

UCLASS()
class TUI2023_API AProjectile3D : public AActor
{
	GENERATED_BODY()

public:
	AProjectile3D();

	UFUNCTION(BlueprintCallable, Category = "Custom Initialization")
	void InitializeProjectile3D(FProjectileParams ProjectileParams, ATarget3D* TargetRef, float g, float SimulationSpeed);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

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
	float RotationSpeedPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotationSpeedYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaitTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> Path = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator DesiredRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> MarkerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATarget3D* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float G = 9.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SimulationSpeedMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FuelTank = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FuelExpense = 0.f;
};
