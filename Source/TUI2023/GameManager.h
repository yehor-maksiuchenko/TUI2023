#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"

UCLASS()
class TUI2023_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	float FlightTime(float u, float angle, float y);
	float MaxHeight(float u);
	void ParabolaPoint2D(float u, float x, float y, float& angle1, float& angle2, float& time1, float& time2);
	void ParabolaPoint3D(float u, FVector Position, float& horizontal_angle, float& vertical_angle1, float& vertical_angle2, float& time1, float& time2);


	UFUNCTION(BlueprintCallable)
	bool ParabolaParabola2D(FVector ProjectilePosition, float ProjectileRotation, float ProjectileVelocity, float RotationSpeed, FVector TargetPosition, float TargetRotation, float TargetVelocity, float Step, float& ResultAngle, float& WaitTime, float& CollisionTime, FVector& CollisionPosition);

	UFUNCTION(BlueprintCallable)
	bool ParabolaParabola3D(FVector ProjectileLocation, float ProjectileYaw, float ProjectilePitch, float YawRotationSpeed, float PitchRotationSpeed, float ProjectileVelocity, FVector TargetLocation, float TargetYaw, float TargetPitch, float TargetVelocity, float& ResultYaw, float& ResultPitch, float Step, float& CollisionTime, float& WaitTime);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="World Attractional Force")
	float G = 9.8f;
};
