#include "Target2D.h"

ATarget2D::ATarget2D()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = Mesh;
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetSphereRadius(100);
	Sphere->SetRelativeLocation(FVector(0, 0, 0));
	Sphere->SetupAttachment(RootComponent);

	Mesh->SetWorldScale3D(FVector(1.f));

	TargetPath.Add(FVector(1230, 0, 620));
	TargetPath.Add(FVector(-2180, 0, 530));
	TargetPath.Add(FVector(-3480, 0, 680));
	TargetPath.Add(FVector(-540, 0, 100));
}

void ATarget2D::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < TargetPath.Num(); i++)
	{
		GetWorld()->SpawnActor<AActor>(MarkerClass, TargetPath[i], FRotator(0, 0, 0));
	}

	if (isBallistic)
	{
		SetActorLocation(FVector(StartLocation.X, 0.f, StartLocation.Z));
		SetActorRotation(StartRotation);
	}
	else
	{
		SetActorLocation(StartLocation);
		ToRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetPath[0]);
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetPath[0]));
	}
}

void ATarget2D::Tick(float DeltaTime)
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
		float dVelocity = Velocity * DeltaTime;
		SetActorLocation(old_pos + GetActorForwardVector() * dVelocity);
		if (FVector::Dist(GetActorLocation(), TargetPath[0]) <= dVelocity)
		{
			if (TargetPath[0] != TargetPath.Last())
			{
				SetActorLocation(TargetPath[0]);
				TargetPath.RemoveAt(0);
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Reached a point: ") + GetActorLocation().ToString());
			}
			else
			{
				Destroy();
			}
		}
		if (bRotate)
		{
			ToRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetPath[0]);
		}
		AerodynamicalRotation(DeltaTime);

	}
}

FVector ATarget2D::BallisticMovement()
{
	float t = GetWorld()->GetTimeSeconds() - TrajectoryStartMoment;

	float Pitch = StartRotation.Pitch * PI / 180.0f;
	float Yaw = StartRotation.Yaw * PI / 180.0f;

	float V0x = Velocity * FMath::Cos(Pitch) * FMath::Cos(Yaw);
	float V0y = Velocity * FMath::Cos(Pitch) * FMath::Sin(Yaw);
	float V0z = Velocity * FMath::Sin(Pitch);

	float X = V0x * t + StartLocation.X;
	float Y = V0y * t + StartLocation.Y;
	float Z = V0z * t - 0.5f * t * t + StartLocation.Z;

	return FVector(X, Y, Z);
}

void ATarget2D::AerodynamicalRotation(float DeltaTime)
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

float Rounding(float x, float r) {
	return FMath::RoundToFloat(x / r) * r;
}

float TimeOfFlight(float u, float angle, float y) {
	double u_y = u * FMath::Sin(FMath::DegreesToRadians(angle));
	double D = u_y * u_y + 2 * G * y;
	double x1 = (-u_y + FMath::Sqrt(D)) / (-G);
	double x2 = (-u_y - FMath::Sqrt(D)) / (-G);

	return x2;

}