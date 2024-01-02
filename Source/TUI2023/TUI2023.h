// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
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
	float RotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	FRotator DesiredRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	float WaitTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	float LauncherRotationSpeedYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	float LauncherRotationSpeedPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	float SizeK = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	float FuelTank = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	float FuelExpense = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	bool isBallistic;
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
	float Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	float RotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	TArray<FVector> Path = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	float SizeK = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	float FuelTank = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	float FuelExpense = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialize")
	bool isBallistic;
};

