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

void ATarget3D::BeginPlay()
{
	Super::BeginPlay();
	SetActorLocation(StartPos);
	SetActorRotation(StartRot);
	CurrentVelocity = StartRot.Vector() * InitVelocity;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, (StartRot.Vector()).ToString());
}

void ATarget3D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (isBallistic)
	{
		t += DeltaTime;
		FVector old_pos = GetActorLocation();
		CurrentVelocity += BallisticMovement(DeltaTime);
		SetActorLocation(old_pos + CurrentVelocity);
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(old_pos, GetActorLocation()));
		/*SetActorLocation(old_pos + BallisticMovement(t * 10));
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(old_pos, GetActorLocation())); */
	}
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, GetActorLocation().ToString());
}

FVector ATarget3D::BallisticMovement(float D_Time)
{
	/*float Pitch = StartRot.Pitch * PI / 180.0f;
	float Yaw = StartRot.Yaw * PI / 180.0f;

	float V0x = InitVelocity * FMath::Cos(Pitch) * FMath::Cos(Yaw);
	float V0y = InitVelocity * FMath::Cos(Pitch) * FMath::Sin(Yaw);
	float V0z = InitVelocity * FMath::Sin(Pitch);

	float X = V0x * Time + StartPos.X;
	float Z = V0z * Time - 0.5f * Time * Time + StartPos.Z;
	float Y = V0y * Time + StartPos.Y;*/

	FVector drag = -CurrentVelocity.GetSafeNormal() * K * CurrentVelocity.SizeSquared() * p;
	FVector gravity = FVector(0.0, 0.0, -G * M);
	FVector force = drag + gravity;
	FVector acc = force / M;
	return acc * D_Time;
}

