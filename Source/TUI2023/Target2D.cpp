#include "Target2D.h"

ATarget2D::ATarget2D()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = Mesh;

	Mesh->SetWorldScale3D(FVector(1.f));

	TargetPath.Add(FVector(-150.0f, -200.0f, 210.0f));
	TargetPath.Add(FVector(680, 820, 300));
	TargetPath.Add(FVector(-1480, 550, 1040));
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
		//bMove = false;
		SetActorLocation(FVector(StartLocation.X, 0.0f, StartLocation.Z));
		StartRotation = FRotator(StartRotation.Pitch, 0.0f, 0.0f);
		SetActorRotation(StartRotation);
		CurrentVelocity = StartRotation.Vector() * Velocity;
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
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, (-CurrentVelocity.GetSafeNormal()).ToString());
	FVector old_pos = GetActorLocation();
	if (isBallistic && bMove)
	{
		t += DeltaTime;
		CurrentVelocity += BallisticMovement() * DeltaTime;
		SetActorLocation(old_pos + CurrentVelocity);
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(old_pos, GetActorLocation()));
	}
	else if(bMove)
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
	/*FVector drag = -CurrentVelocity.GetSafeNormal() * K * CurrentVelocity.SizeSquared() * p;
	FVector gravity = FVector(0.0, 0.0, -G * M);
	FVector force = drag + gravity;
	FVector acc = force / M;*/
	return (-CurrentVelocity.GetSafeNormal() * K * CurrentVelocity.SizeSquared() * p + FVector(0.0, 0.0, -G * M)) / M;
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

