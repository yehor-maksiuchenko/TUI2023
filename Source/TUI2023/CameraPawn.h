#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ArrowComponent.h"
#include "CameraPawn.generated.h"

UCLASS()
class TUI2023_API ACameraPawn : public APawn
{
	GENERATED_BODY()

public:
	ACameraPawn();

protected:
	virtual void BeginPlay() override;

	/*void RotateHorizontal(float Value);
	void RotateVertical(float Value);*/
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Zoom(float Value);
	void RotateRight(float Value);
	void RotateLeft(float Value);
	/*void EnableRotation();
	void DisableRotation();*/

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCameraComponent* CamComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* MainComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UArrowComponent* ArrowComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MinZoom = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxZoom = 4000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ZoomSpeed = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MoveSpeed = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RotateSpeed = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float PitchMax = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float PitchMin = 10.0f;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCanRotate = false;*/

private:
	UFUNCTION()
		void CameraBounds();

	UPROPERTY()
		FVector TargetLocation;

	UPROPERTY()
		FRotator TargetRotation;

	UPROPERTY()
		float TargetZoom;
};
