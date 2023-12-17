#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "CameraPawn2D.generated.h"

UCLASS()
class TUI2023_API ACameraPawn2D : public APawn
{
	GENERATED_BODY()

public:
	ACameraPawn2D();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveUp(float Value);
	void MoveRight(float Value);
	void Zoom(float Value);
	void Bounds();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USceneComponent* MainComp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		USpringArmComponent* CameraBoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UCameraComponent* CamComp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MaxZoom = 2000.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MinZoom = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float ZoomSpeed = 2.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MoveSpeed = 20.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float CurCur = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MinHeight = 30.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MaxHeight = 800.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MinWidth = -500.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MaxWidth = 500.0f;

private:
	UPROPERTY()
		float TargetZoom;

	UPROPERTY()
		FVector TargetLocation;
};
