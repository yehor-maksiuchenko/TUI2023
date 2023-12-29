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

	TargetPath.Add(FVector(1230, 0, 620));
	TargetPath.Add(FVector(-2180, 0, 530));
	TargetPath.Add(FVector(-3480, 0, 680));
	TargetPath.Add(FVector(-540, 0, 100));
}

void AProjectile2D::BeginPlay()
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

void AProjectile2D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector old_pos = GetActorLocation();
	if (isBallistic)
	{
		if (bMove)
		{
			SetActorLocation(BallisticMovement());
			SetActorRotation(UKismetMathLibrary::FindLookAtRotation(old_pos, GetActorLocation()));
		}
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

FVector AProjectile2D::BallisticMovement()
{
	float t = GetWorld()->GetTimeSeconds() - TrajectoryStartMoment;
	//t *= 10;

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

float AProjectile2D::FlightTime(float u, float angle, float y) {
	float u_y = u * FMath::Sin(FMath::DegreesToRadians(angle));
	float D = u_y * u_y + 2 * G * y;
	float x1 = (-u_y + FMath::Sqrt(D)) / (-G);
	float x2 = (-u_y - FMath::Sqrt(D)) / (-G);

	return x2;
}

void AProjectile2D::ParabolaPoint(float u, float x, float y, float& angle1, float& angle2, float& time1, float& time2)
{
	float a = -G * 0.5 * FMath::Square(x / u);
	float b = x;
	float c = a - y;

	float D = FMath::Square(b) - 4 * a * c;

	if (D > 0) {
		float t1 = (-1 * b + FMath::Sqrt(D)) / (2 * a);
		float t2 = (-1 * b - FMath::Sqrt(D)) / (2 * a);

		angle1 = FMath::Atan(t1) * 180 / pi;
		angle2 = FMath::Atan(t2) * 180 / pi;

		time1 = x / (u * FMath::Cos(FMath::DegreesToRadians(angle1)));
		time2 = x / (u * FMath::Cos(FMath::DegreesToRadians(angle2)));
	}
	else if (D == 0) {
		float t1 = (-1 * b + FMath::Sqrt(D)) / (2 * a);
		float t2 = (-1 * b - FMath::Sqrt(D)) / (2 * a);

		angle1 = FMath::Atan(t1) * 180 / pi;
		angle2 = FMath::Atan(t2) * 180 / pi;

		time1 = x / (u * FMath::Cos(FMath::DegreesToRadians(angle1)));
		time2 = x / (u * FMath::Cos(FMath::DegreesToRadians(angle2)));
	}
	else {
		angle1 = -1;
		angle2 = -1;
		time1 = -1;
		time2 = -1;
	}
}

void AProjectile2D::PredictTrajectory(float v1 /*Self velocity*/, float v2 /*Target velocity*/, float angle2 /*Target Pitch*/, float x_0, float y_0, float& result_angle, float Step, /*float& CollisionTime,*/ float& WaitTime/*, TArray<float>& CollisionPosition*/)
{
	float u_x2 = v2 * FMath::Cos(FMath::DegreesToRadians(angle2));
	float u_y2 = v2 * FMath::Sin(FMath::DegreesToRadians(angle2));
	float Time = FlightTime(v2, angle2, y_0);
	//CollisionTime = -1;
	WaitTime = -1;
	result_angle = -1;
	//CollisionPosition[0] = -1;
	//CollisionPosition[1] = -1;

	for (float t = 0; t < Time; t += Step) {
		float s_x = u_x2 * t;
		float s_y = u_y2 * t - 0.5 * G * t * t;
		float x = x_0 + s_x;
		float y = y_0 + s_y;

		float angle1, angle2, time1, time2;
		ParabolaPoint(v1, x, y, angle1, angle2, time1, time2);
		if (time1 <= t) {
			result_angle = angle1;
			//CollisionTime = t;
			WaitTime = t - time1;
			//CollisionPosition[0] = x;
			//CollisionPosition[1] = y;
			break;
		}
		if (time2 <= t) {
			result_angle = angle2;
			//CollisionTime = t;
			WaitTime = t - time2;
			//CollisionPosition[0] = x;
			//CollisionPosition[1] = y;
			break;
		}
	}
}
