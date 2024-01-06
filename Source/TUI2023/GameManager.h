#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TUI2023.h"
#include "Target2D.h"
#include "Target3D.h"
#include "Projectile2D.h"
#include "Projectile3D.h"
#include "GameManager.generated.h"


UCLASS()
class TUI2023_API UGameManager : public UGameInstance
{
	GENERATED_BODY()

public:
	UGameManager();

protected:
	//virtual void BeginPlay() override;

public:
	//virtual void Tick(float DeltaTime) override;

	float FlightTime(float u, float angle, float y);
	float MaxHeight(float u);
	float GetAngleOnThePlane(float x, float y);
	void ParabolaPoint2D(float u, FVector2D& Position, float& angle1, float& angle2, float& time1, float& time2);
	void ParabolaPoint3D(float u, FVector& Position, float& horizontal_angle, float& vertical_angle1, float& vertical_angle2, float& time1, float& time2);


	UFUNCTION(BlueprintCallable, meta = (Step = 0.1))
	bool ParabolaParabola2D(UPARAM(ref, DisplayName = "Projectile Parameters") FProjectileParams& ProjectileParams, UPARAM(ref, DisplayName = "Target Parameters") FTargetParams& TargetParams, float Step, float& CollisionTime, FVector& CollisionPosition);

	UFUNCTION(BlueprintCallable, meta = (Step = 0.1))
	bool ParabolaParabola3D(UPARAM(ref, DisplayName = "Projectile Parameters") FProjectileParams& ProjectileParams, UPARAM(ref, DisplayName = "Target Parameters") FTargetParams& TargetParams, float Step, float& CollisionTime);

	UFUNCTION(BlueprintPure)
	void SetPredictionResults(FRotator DesiredRotation, float WaitTime, FProjectileParams& ProjectileParams);
	
	UFUNCTION(BlueprintCallable)
	void ResetSimulation();

	UFUNCTION(BlueprintCallable)
	void LoadSimulation2D();

	UFUNCTION(BlueprintCallable)
	void LoadSimulation3D();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "World Settings")
	float G = 9.8f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "World Settings")
	float WorldSimulationSpeed = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lists of Objects")
	TArray<ATarget2D*> Targets2D = {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lists of Objects")
	TArray<AProjectile2D*> Projectiles2D = {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lists of Objects")
	TArray<ATarget3D*> Targets3D = {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lists of Objects")
	TArray<AProjectile3D*> Projectiles3D = {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lists of Objects Specs")
	TArray<FTargetParams> TargetsToLoad = {};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lists of Objects Specs")
	TArray<FProjectileParams> ProjectilesToLoad = {};
};

