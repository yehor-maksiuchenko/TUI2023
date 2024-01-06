#include "Target3D.h"

ATarget3D::ATarget3D()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = Mesh;
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));

	Mesh->SetRelativeLocation(FVector(0, 0, 0));
	Mesh->SetRelativeRotation(FRotator(0, 0, 0));

	Sphere->SetupAttachment(RootComponent);
}

void ATarget3D::InitializeTarget3D(FTargetParams TargetParams, float g, float SimulationSpeed)
{
	isBallistic = TargetParams.isBallistic;
	StartLocation = TargetParams.StartLocation;
	StartRotation = TargetParams.StartRotation;
	Velocity = TargetParams.Velocity;
	RotationSpeed = TargetParams.RotationSpeed;
	Mesh->SetStaticMesh(TargetParams.Mesh);
	SimulationSpeedMultiplier = SimulationSpeed;
	G = g;
	Mesh->SetWorldScale3D(FVector(TargetParams.SizeK));
	Sphere->SetSphereRadius(TargetParams.SizeK);
	SetActorLocation(StartLocation);
	SetActorRotation(StartRotation);
	FuelTank = TargetParams.FuelTank;
	FuelExpense = TargetParams.FuelExpense;
	if (!isBallistic)
	{
		Path = TargetParams.Path;
		DesiredRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Path[0]);
	}
}

void ATarget3D::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < Path.Num(); i++)
	{
		GetWorld()->SpawnActor<AActor>(MarkerClass, Path[i], FRotator(0, 0, 0));
	}
}

void ATarget3D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DeltaTime *= SimulationSpeedMultiplier;

	FVector old_pos = GetActorLocation();
	if (isBallistic)
	{
		SetActorLocation(BallisticMovement());
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(old_pos, GetActorLocation()));
	}
	else
	{
		float dVelocity = Velocity * DeltaTime;
		SetActorLocation(old_pos + GetActorForwardVector() * dVelocity);
		FuelTank -= FuelExpense * DeltaTime;
		if (FuelTank <= 0) { isBallistic = true; StartLocation = GetActorLocation(); StartRotation = GetActorRotation(); }
		if (FVector::Dist(GetActorLocation(), Path[0]) <= dVelocity)
		{
			SetActorLocation(Path[0]);
			if (Path[0] != Path.Last())
			{
				SetActorLocation(Path[0]);
				Path.RemoveAt(0);
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Reached a point: ") + GetActorLocation().ToString());
			}
			else
			{
				isBallistic = true; StartLocation = GetActorLocation(); StartRotation = GetActorRotation();
			}
		}
		if (bRotate)
		{
			DesiredRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Path[0]);
		}
		AerodynamicalRotation(DeltaTime);
		
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
	if (CurrentRotation != DesiredRotation)
	{
		bRotate = true;
		float DotProduct = FMath::Abs(DesiredRotation.Quaternion().GetNormalized() | CurrentRotation.Quaternion().GetNormalized());
		float AngularDifference = FMath::RadiansToDegrees(2.0f * FMath::Acos(DotProduct));
		FQuat ResultRotation = FQuat::Slerp(CurrentRotation.Quaternion(), DesiredRotation.Quaternion(), FMath::Clamp((DeltaTime * RotationSpeed) / AngularDifference, 0.0f, 1.0f));
		SetActorRotation(ResultRotation);
	}
	else bRotate = false;
}

