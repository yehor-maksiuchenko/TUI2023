// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TUI2023.generated.h"

USTRUCT(BlueprintType)
struct FProjectileParams
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	FVector StartLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	FRotator StartRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	float Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	bool isBallistic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	FRotator DesiredRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	float RotationSpeedYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	float RotationSpeedPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	float G = 9.8f;
};

USTRUCT(BlueprintType)
struct FTargetParams
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	FVector StartLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	FRotator StartRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	bool isBallistic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	float Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	float RotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	TArray<FVector> Path;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	float G = 9.8f;
};

