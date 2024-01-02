#include "Projectile2D.h"

AProjectile2D::AProjectile2D()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = Mesh;
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetSphereRadius(100);
	Sphere->SetRelativeLocation(FVector(0, 0, 0));
	Sphere->SetupAttachment(RootComponent);

	Mesh->SetWorldScale3D(FVector(1.f));
}

void AProjectile2D::InitializeProjectile2D(FProjectileParams ProjectileParams, ATarget2D* TargetRef, float g, float SimulationSpeed)
{
	isBallistic = ProjectileParams.isBallistic;
	StartLocation = ProjectileParams.StartLocation;
	StartRotation = ProjectileParams.StartRotation;
	Velocity = ProjectileParams.Velocity;
	RotationSpeed = ProjectileParams.RotationSpeed;
	SimulationSpeedMultiplier = SimulationSpeed;
	G = g;
	SetActorLocation(FVector(StartLocation.X, 0.f, StartLocation.Z));
	SetActorRotation(StartRotation);
	if (isBallistic)
	{
		WaitTime = ProjectileParams.WaitTime;
		DesiredRotation = ProjectileParams.DesiredRotation;
	}
	else
	{
		Target = TargetRef;
		Path.Add(Target->GetActorLocation());
		DesiredRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Path[0]);
	}
	if (ProjectileParams.WaitTime > 0.f) bWait = true;

	else
	{
		
	}
}

void AProjectile2D::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < Path.Num(); i++)
	{
		GetWorld()->SpawnActor<AActor>(MarkerClass, Path[i], FRotator(0, 0, 0));
	}
}

void AProjectile2D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector old_pos = GetActorLocation();
	if (isBallistic)
	{
		if (bWait)
		{
			if (GetActorRotation() != DesiredRotation)
			{
				AerodynamicalRotation(DeltaTime * SimulationSpeedMultiplier);
			}
			if (GetGameTimeSinceCreation() * SimulationSpeedMultiplier >= WaitTime * SimulationSpeedMultiplier) bWait = false;
		}
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
			DesiredRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Path[0]);
		}
		AerodynamicalRotation(DeltaTime * SimulationSpeedMultiplier);

	}
}

FVector AProjectile2D::BallisticMovement()
{
	float t = GetGameTimeSinceCreation() * SimulationSpeedMultiplier;

	float Pitch = StartRotation.Pitch * PI / 180.0f;
	float Yaw = StartRotation.Yaw * PI / 180.0f;

	float V0x = Velocity * FMath::Cos(Pitch) * FMath::Cos(Yaw);
	float V0y = Velocity * FMath::Cos(Pitch) * FMath::Sin(Yaw);
	float V0z = Velocity * FMath::Sin(Pitch);

	float X = V0x * t + StartLocation.X;
	float Y = V0y * t + StartLocation.Y;
	float Z = V0z * t - 0.5f * t * G * t + StartLocation.Z;

	return FVector(X, Y, Z);
}

void AProjectile2D::AerodynamicalRotation(float DeltaTime)
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