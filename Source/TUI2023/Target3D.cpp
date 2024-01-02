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

void ATarget3D::InitializeTarget3D(FTargetParams TargetParams, float g, float SimulationSpeed)
{
	isBallistic = TargetParams.isBallistic;
	StartLocation = TargetParams.StartLocation;
	StartRotation = TargetParams.StartRotation;
	Velocity = TargetParams.Velocity;
	RotationSpeed = TargetParams.RotationSpeed;
	SimulationSpeedMultiplier = SimulationSpeed;
	G = g;
	SetActorLocation(StartLocation);
	if (isBallistic)
	{
		SetActorRotation(StartRotation);
	}
	else
	{
		Path = TargetParams.Path;
		ToRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Path[0]);
		SetActorRotation(ToRotation);
	}
}

void ATarget3D::BeginPlay()
{
	Super::BeginPlay();
}

void ATarget3D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector old_pos = GetActorLocation();
	if (isBallistic)
	{
		SetActorLocation(BallisticMovement());
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(old_pos, GetActorLocation()));
	}
	else
	{
		float dVelocity = Velocity * DeltaTime * SimulationSpeedMultiplier;
		SetActorLocation(old_pos + GetActorForwardVector() * dVelocity);
		if (FVector::Dist(GetActorLocation(), Path[0]) <= dVelocity)
		{
			if (Path[0] != Path.Last())
			{
				SetActorLocation(Path[0]);
				Path.RemoveAt(0);
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Reached a point: ") + GetActorLocation().ToString());
			}
			else
			{
				Destroy();
			}
		}
		if (bRotate)
		{
			ToRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Path[0]);
		}
		AerodynamicalRotation(DeltaTime * SimulationSpeedMultiplier);
		
	}
}

FVector ATarget3D::BallisticMovement()
{
	float t = GetGameTimeSinceCreation() * SimulationSpeedMultiplier;

	float Pitch = StartRotation.Pitch * PI / 180.0f;
	float Yaw = StartRotation.Yaw * PI / 180.0f;

	float V0x = Velocity * FMath::Cos(Pitch) * FMath::Cos(Yaw);
	float V0y = Velocity * FMath::Cos(Pitch) * FMath::Sin(Yaw);
	float V0z = Velocity * FMath::Sin(Pitch);

	float X = V0x * t + StartLocation.X;
	float Y = V0y * t + StartLocation.Y;
	float Z = V0z * t - 0.5f * t * t * G + StartLocation.Z;

	return FVector(X, Y, Z);
}

void ATarget3D::AerodynamicalRotation(float DeltaTime)
{
	FRotator CurrentRotation = GetActorRotation();
	if (CurrentRotation != ToRotation)
	{
		bRotate = true;
		float DotProduct = FMath::Abs(ToRotation.Quaternion().GetNormalized() | CurrentRotation.Quaternion().GetNormalized());
		float AngularDifference = FMath::RadiansToDegrees(2.0f * FMath::Acos(DotProduct));
		FQuat ResultRotation = FQuat::Slerp(CurrentRotation.Quaternion(), ToRotation.Quaternion(), FMath::Clamp((DeltaTime * RotationSpeed) / AngularDifference, 0.0f, 1.0f));
		SetActorRotation(ResultRotation);
	}
	else bRotate = false;
}

