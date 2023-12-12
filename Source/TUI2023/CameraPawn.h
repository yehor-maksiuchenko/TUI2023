#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
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

	void TurnCamera(float Value);
	void MoveForward(float Value);
	void MoveRight(float Value);
	void LiftCamera(float Value);

	const float LowestCameraHight = 200.0f;
	const float HighestCameraHight = 1000.0f;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCameraComponent* CamComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* MainComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UArrowComponent* ArrowComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USpringArmComponent* CameraBoom;
};
