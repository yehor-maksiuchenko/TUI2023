#pragma once
//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("hohoho"));

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Containers/Array.h" 
#include "Math/Quat.h"
#include "Math/Vector.h"
#include "Components/SphereComponent.h"
#include "TUI2023.h"
#include "Kismet/KismetMathLibrary.h"
#include "Target3D.generated.h"

UCLASS()
class TUI2023_API ATarget3D : public AActor
{
	GENERATED_BODY()
	
public:	
	ATarget3D();

	UFUNCTION(BlueprintCallable, Category = "Custom Initialization")
	void InitializeTarget3D(FTargetParams TargetParams, float g, float SimulationSpeed);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	bool bRotate = false;
	int CollidingObjects = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* Mesh;

	UFUNCTION()
	void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* Sphere;

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	FVector BallisticMovement();

	UFUNCTION()
	void AerodynamicalRotation(float DeltaTime);

	UFUNCTION()
	void ObstacleAvoidance(float DeltaTime);

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
	TArray<FVector> Path = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> MarkerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator DesiredRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float G = 9.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SimulationSpeedMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FuelTank = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FuelExpense = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldManeuver = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AvoidanceAngleStep = 10.0f; // degrees

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AvoidanceDensityStep = 5.f; // degrees

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SizeK = 1.f;
};
