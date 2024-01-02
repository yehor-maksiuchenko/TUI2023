#pragma once
//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("hohoho"));

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Containers/Array.h" 
#include "Math/Quat.h"
#include "Math/Vector.h"
#include "TUI2023.h"
#include "Kismet/KismetMathLibrary.h"
#include "Target3D.generated.h"

UCLASS()
class TUI2023_API ATarget3D : public AActor
{
	GENERATED_BODY()
	
public:	
	ATarget3D();

	UFUNCTION(BlueprintCallable, Category = "Custom Initialization", meta = (AutoCreateRefTerm = "Path", g = 9.8, SimulationSpeed = 1.0))
	void InitializeTarget3D(FTargetParams TargetParams, float g, float SimulationSpeed);

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
		FRotator ToRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float G = 9.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SimulationSpeedMultiplier = 1.f;
};
