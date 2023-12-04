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
		ToLocation = TargetPath[0];
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(StartLocation, ToLocation));
	}
	
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, (StartRot.Vector()).ToString());
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
		if (FVector::Dist(GetActorLocation(), ToLocation) <= dVelocity)
		{
			if (ToLocation != TargetPath.Last())
			{
				SetActorLocation(ToLocation);
				//SetActorRotation(UKismetMathLibrary::FindLookAtRotation(ToLocation, TargetPath[1]));
				ToLocation = TargetPath[1];
				TargetPath.RemoveAt(0);
				bRotate = true;
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Reached a point: ") + GetActorLocation().ToString());
			}
			else
			{
				Velocity = 0;
				TargetPath.RemoveAt(0);
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Reached last point"));
				bRotate = false;
			}
		}
		if (bRotate)
		{
			AerodynamicalRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ToLocation), DeltaTime);
			//AddActorLocalRotation((UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ToLocation) - GetActorRotation()).GetNormalized() * DeltaTime * RotationSpeed);
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(1.0f / DeltaTime));
}

FVector ATarget3D::BallisticMovement()
{
	/*FVector drag = -CurrentVelocity.GetSafeNormal() * K * CurrentVelocity.SizeSquared() * p;
	FVector gravity = FVector(0.0, 0.0, -G * M);
	FVector force = drag + gravity;
	FVector acc = force / M;*/
	return (-CurrentVelocity.GetSafeNormal() * K * CurrentVelocity.SizeSquared() * p + FVector(0.0, 0.0, -G * M)) / M;
}

void ATarget3D::AerodynamicalRotation(const FRotator& TargetRotation, float DeltaTime)
{
	FRotator CurrentRotation = GetActorRotation();
	if (CurrentRotation != TargetRotation)
	{
		FRotator DeltaRotation = FMath::RInterpConstantTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);

		//FRotator NewRotation = FMath::Lerp(CurrentRotation, TargetRotation, FMath::Clamp(RotationSpeed * DeltaTime, 0.0f, 1.0f));

		SetActorRotation(DeltaRotation);//NewRotation);
	}
	else
	{
		bRotate = false;
	}
}

