#include "Projectile3D.h"

AProjectile3D::AProjectile3D()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = Mesh;
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetRelativeLocation(FVector(0, 0, 0));
	Sphere->SetupAttachment(RootComponent);
}

void AProjectile3D::SetPredictionResults(FRotator Rotation, float Time)
{
	
}

void AProjectile3D::InitializeProjectile3D(FProjectileParams ProjectileParams, ATarget3D* TargetRef, float g, float SimulationSpeed)
{
	isBallistic = ProjectileParams.isBallistic;
	if (isBallistic)
	{
		DesiredRotation = ProjectileParams.DesiredRotation;
		WaitTime = ProjectileParams.WaitTime;
	}
	else
	{
		Target = TargetRef;
		Path.Add(Target->GetActorLocation());
		DesiredRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Path[0]);
	}
	/*StartLocation = ProjectileParams.StartLocation;
	StartRotation = ProjectileParams.StartRotation;*/
	Velocity = ProjectileParams.Velocity;
	RotationSpeed = ProjectileParams.RotationSpeed;
	RotationSpeedPitch = ProjectileParams.LauncherRotationSpeedPitch;
	RotationSpeedYaw = ProjectileParams.LauncherRotationSpeedYaw;
	Mesh->SetStaticMesh(ProjectileParams.Mesh);
	SimulationSpeedMultiplier = SimulationSpeed;
	G = g;
	SetActorLocation(StartLocation);
	SetActorRotation(StartRotation);
	Mesh->SetWorldScale3D(FVector(ProjectileParams.SizeK));
	Sphere->SetSphereRadius(ProjectileParams.SizeK);
	FuelTank = ProjectileParams.FuelTank;
	FuelExpense = ProjectileParams.FuelExpense;
	if (ProjectileParams.WaitTime > 0.f) bWait = true;
}

void AProjectile3D::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < Path.Num(); i++)
	{
		GetWorld()->SpawnActor<AActor>(MarkerClass, Path[i], FRotator(0, 0, 0));
	}
}

void AProjectile3D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DeltaTime *= SimulationSpeedMultiplier;

	FVector old_pos = GetActorLocation();
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, old_pos.ToString());
	if (isBallistic)
	{
		if (bWait)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(DeltaTime));

			RotationWhileWaiting(DeltaTime);
			if (GetGameTimeSinceCreation() >= WaitTime) { bWait = false; StartLocation = GetActorLocation(); StartRotation = GetActorRotation(); }
		}
		else
		{
			SetActorLocation(BallisticMovement());
			SetActorRotation(UKismetMathLibrary::FindLookAtRotation(old_pos, GetActorLocation()));
		}
	}
	else
	{
		float dVelocity = (GetGameTimeSinceCreation() >= 1 ? Velocity * DeltaTime: Velocity * DeltaTime * GetGameTimeSinceCreation());
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

FVector AProjectile3D::BallisticMovement()
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

void AProjectile3D::AerodynamicalRotation(float DeltaTime)
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

void AProjectile3D::RotationWhileWaiting(float DeltaTime)
{
	FRotator CurrentRotation = GetActorRotation();
	if (CurrentRotation.Pitch != DesiredRotation.Pitch)
	{
		if (FMath::Abs(DesiredRotation.Pitch - CurrentRotation.Pitch) > RotationSpeedPitch * DeltaTime)
		{
			float NewPitch;
			if (DesiredRotation.Pitch > CurrentRotation.Pitch)
			{
				NewPitch = CurrentRotation.Pitch + (RotationSpeedPitch * DeltaTime);
			}
			else
			{
				NewPitch = CurrentRotation.Pitch - (RotationSpeedPitch * DeltaTime);
			}
			SetActorRotation(FRotator(NewPitch, CurrentRotation.Yaw, CurrentRotation.Roll));
		}
		else SetActorRotation(FRotator(DesiredRotation.Pitch, CurrentRotation.Yaw, CurrentRotation.Roll));
	}
	CurrentRotation = GetActorRotation();
	if (CurrentRotation.Yaw != DesiredRotation.Yaw)
	{
		if (FMath::Abs(DesiredRotation.Yaw - CurrentRotation.Yaw) > RotationSpeedYaw * DeltaTime)
		{
			float NewYaw = (DesiredRotation.Yaw > CurrentRotation.Yaw ? CurrentRotation.Yaw + RotationSpeedYaw * DeltaTime : CurrentRotation.Yaw - RotationSpeedYaw * DeltaTime);
			SetActorRotation(FRotator(CurrentRotation.Pitch, NewYaw, CurrentRotation.Roll));
		}
		else SetActorRotation(FRotator(CurrentRotation.Pitch, DesiredRotation.Yaw, CurrentRotation.Roll));
	}
}
