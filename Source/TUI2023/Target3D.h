#pragma once
//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("hohoho"));

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Target3D.generated.h"

UCLASS()
class TUI2023_API ATarget3D : public AActor
{
	GENERATED_BODY()
	
public:	
	ATarget3D();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	float t = 0;

	float ENG = 40.0;
	float M = 1.0;
	float G = 9.8;
	float K = 0.05; //change this later

	FVector BallisticMovement(float Time);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bMove = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isBallistic = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector StartPos = FVector(460, -350, 80);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator StartRot = FRotator(40, 0, 90);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int InitVelocity = 50;


};
