#include "Target3D.h"

ATarget3D::ATarget3D()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = Mesh;

	Mesh->SetWorldScale3D(FVector(1.f));
	Mesh->SetRelativeLocation(FVector(0, 0, 0));
	Mesh->SetRelativeRotation(FRotator(0, 0, 0));

	TargetPath.Add(FVector(-150.0f, -200.0f, 210.0f));
	TargetPath.Add(FVector(680, 820, 300));
	TargetPath.Add(FVector(-1480, 550, 1040));
}

void ATarget3D::BeginPlay()
{
	Super::BeginPlay();
	SetActorLocation(StartLocation);
	if (isBallistic)
	{
		SetActorRotation(StartRotation);
		CurrentVelocity = StartRotation.Vector() * Velocity;
	}
	else
	{
		ToRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetPath[0]);
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetPath[0]));
	}
}

void ATarget3D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector old_pos = GetActorLocation();
	if (isBallistic)
	{
		t += DeltaTime;
		CurrentVelocity += BallisticMovement() * DeltaTime;
		SetActorLocation(old_pos + CurrentVelocity);
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

FVector ATarget3D::BallisticMovement()
{
	/*FVector drag = -CurrentVelocity.GetSafeNormal() * K * CurrentVelocity.SizeSquared() * p;
	FVector gravity = FVector(0.0, 0.0, -G * M);
	FVector force = drag + gravity;
	FVector acc = force / M;*/
	return (-CurrentVelocity.GetSafeNormal() * K * CurrentVelocity.SizeSquared() * p + FVector(0.0, 0.0, -G * M)) / M;
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

