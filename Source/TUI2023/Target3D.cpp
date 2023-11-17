#include "Target3D.h"

ATarget3D::ATarget3D()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = Mesh;

	Mesh->SetWorldScale3D(FVector(1.f));
	Mesh->SetRelativeLocation(FVector(0, 0, 0));
	Mesh->SetRelativeRotation(FRotator(0, 0, 0));
}

void ATarget3D::BeginPlay()
{
	Super::BeginPlay();
	SetActorLocation(StartPos);
	SetActorRotation(StartRot);
}

void ATarget3D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	t += DeltaTime;
	if (isBallistic)
	{
		FVector old_pos = GetActorLocation();
		SetActorLocation(/*old_pos + */BallisticMovement(t));
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(old_pos, GetActorLocation()));
		
	}
}

FVector ATarget3D::BallisticMovement(float Time)
{
	float Pitch = StartRot.Pitch * PI / 180.0f;
	float Yaw = StartRot.Yaw * PI / 180.0f;

	float V0x = InitVelocity * FMath::Cos(Pitch) * FMath::Cos(Yaw);
	float V0y = InitVelocity * FMath::Cos(Pitch) * FMath::Sin(Yaw);
	float V0z = InitVelocity * FMath::Sin(Pitch);

	float X = V0x * t + StartPos.X;
	float Z = V0z * t - 0.5f * t * t + StartPos.Z;
	float Y = V0y * t + StartPos.Y;
	return FVector(X, Y, Z);
}

