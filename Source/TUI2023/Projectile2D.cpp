#include "Projectile2D.h"

AProjectile2D::AProjectile2D()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = Mesh;
	Mesh->SetWorldScale3D(FVector(1.f));
	Mesh->SetRelativeRotation(FRotator(0.f));

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetRelativeLocation(FVector(0, 0, 0));
	Sphere->SetupAttachment(RootComponent);
}

void AProjectile2D::InitializeProjectile2D(FProjectileParams ProjectileParams, ATarget2D* TargetRef, float g, float SimulationSpeed)
{
	isBallistic = ProjectileParams.isBallistic;
	StartLocation = ProjectileParams.StartLocation;
	StartRotation = ProjectileParams.StartRotation;
	Velocity = ProjectileParams.Velocity;
	RotationSpeed = ProjectileParams.RotationSpeed;
	Mesh->SetStaticMesh(ProjectileParams.Mesh);
	SimulationSpeedMultiplier = SimulationSpeed;
	G = g;
	Mesh->SetWorldScale3D(FVector(ProjectileParams.SizeK));
	Sphere->SetSphereRadius(ProjectileParams.SizeK);
	FuelTank = ProjectileParams.FuelTank;
	FuelExpense = ProjectileParams.FuelExpense;
	SetActorLocation(FVector(StartLocation.X, 0.f, StartLocation.Z));
	SetActorRotation(FRotator(StartRotation.Pitch, 0.f, 0.f));
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
	DeltaTime *= SimulationSpeedMultiplier;

	FVector old_pos = GetActorLocation();
	if (isBallistic)
	{
		if (bWait)
		{
			RotationWhileWaiting(DeltaTime);
			if (GetGameTimeSinceCreation() * SimulationSpeedMultiplier >= WaitTime) { bWait = false; StartRotation = GetActorRotation(); }
		}
		else
		{
			SetActorLocation(BallisticMovement());
			SetActorRotation(UKismetMathLibrary::FindLookAtRotation(old_pos, GetActorLocation()));
		}
	}
	else
	{
		float dVelocity = (GetGameTimeSinceCreation() >= 1 ? Velocity * DeltaTime : Velocity * DeltaTime * GetGameTimeSinceCreation());
		SetActorLocation(old_pos + GetActorForwardVector() * dVelocity);
		FuelTank -= (GetGameTimeSinceCreation() >= 1 ? FuelExpense * DeltaTime : FuelExpense * DeltaTime * 2);
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

FVector AProjectile2D::BallisticMovement()
{
	float t = (GetGameTimeSinceCreation() - WaitTime) * SimulationSpeedMultiplier;

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

void AProjectile2D::RotationWhileWaiting(float DeltaTime)
{
	FRotator CurrentRotation = GetActorRotation();
	if (CurrentRotation.Pitch != DesiredRotation.Pitch)
	{
		bRotate = true;
		float AngularDifference = (FMath::Abs(DesiredRotation.Pitch - CurrentRotation.Pitch) > RotationSpeed * DeltaTime ? RotationSpeed * DeltaTime : DesiredRotation.Pitch - CurrentRotation.Pitch);
		float NewPitch = (DesiredRotation.Pitch > CurrentRotation.Pitch ? AngularDifference : -AngularDifference);
		SetActorRotation(FRotator(CurrentRotation.Pitch + NewPitch, 0.f, 0.f));
	}
	else bRotate = false;
}
