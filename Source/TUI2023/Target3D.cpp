#include "Target3D.h"

ATarget3D::ATarget3D()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = Mesh;
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &ATarget3D::OnProjectileBeginOverlap);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ATarget3D::OnSphereBeginOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &ATarget3D::OnSphereEndOverlap);

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
	SizeK = TargetParams.SizeK;
	Sphere->SetSphereRadius(90 / RotationSpeed * Velocity / TargetParams.SizeK + 10);
	SimulationSpeedMultiplier = SimulationSpeed;
	G = g;
	Mesh->SetWorldScale3D(FVector(TargetParams.SizeK));
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
	UE_LOG(LogTemp, Warning, TEXT("DeltaTime: %f"), DeltaTime);

	FVector old_pos = GetActorLocation();
	if (isBallistic)
	{
		SetActorLocation(BallisticMovement());
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(old_pos, GetActorLocation()));
	}
	else
	{
		float dVelocity = 0;// = Velocity * DeltaTime;
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

		if (CollidingObjects >= 1)
		{
			FHitResult Hit;
			FCollisionQueryParams TraceParams;
			TraceParams.AddIgnoredActor(this);
			if (GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * Sphere->GetScaledSphereRadius(), ECollisionChannel::ECC_GameTraceChannel1, TraceParams))
			{
				DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * Sphere->GetScaledSphereRadius(), Hit.bBlockingHit ? FColor::Orange : FColor::Blue, false, Hit.bBlockingHit ? 4.f : 0.f, 0, 10.0f);
				ObstacleAvoidance(DeltaTime);
			} else DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * Sphere->GetScaledSphereRadius(), Hit.bBlockingHit ? FColor::Orange : FColor::Blue, false, Hit.bBlockingHit ? 4.f : 0.f, 0, 10.0f);
		}
		DesiredRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Path[0]);
		AerodynamicalRotation(DeltaTime);
		//if (bShouldManeuver) ObstacleAvoidance(DeltaTime); else AerodynamicalRotation(DeltaTime);
	}
}

void ATarget3D::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	SetActorTickEnabled(false);
}

void ATarget3D::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CollidingObjects += 1;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::FromInt(CollidingObjects) + TEXT(" Collisions"));
}

void ATarget3D::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CollidingObjects -= 1;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::FromInt(CollidingObjects) + TEXT(" Collisions"));
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
	/*FRotator CurrentRotation = GetActorRotation();
	float RotationSpeedPitch = RotationSpeed * (abs(DesiredRotation.Pitch) - abs(CurrentRotation.Pitch)) / ((abs(DesiredRotation.Pitch) - abs(CurrentRotation.Pitch)) + (abs(DesiredRotation.Yaw) - abs(CurrentRotation.Yaw)));
	float RotationSpeedYaw = RotationSpeed - RotationSpeedPitch;
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
	}*/
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

void ATarget3D::ObstacleAvoidance(float DeltaTime)
{
	FRotator CurrentRotation = GetActorRotation();
	float PitchDifference = DesiredRotation.Pitch - CurrentRotation.Pitch; // y
	float YawDifference = DesiredRotation.Yaw - CurrentRotation.Yaw; // x
	float ArcBaseAngle = 180 - 2 * FMath::Atan(PitchDifference / FMath::Abs(YawDifference));
	float StartAngle = ArcBaseAngle + (180 - ArcBaseAngle) / 2;
	float AngleBetweenRays = ArcBaseAngle / (RaysInArc + 1);
	TArray<FHitResult> LastHits;
	LastHits.AddUninitialized(RaysInArc);
	TArray<FVector> GoodGuesses;
	UWorld* World = GetWorld();
	float Radius = FMath::Sqrt(PitchDifference * PitchDifference + YawDifference * YawDifference);
	//float StartAngle = FMath::Atan2(DesiredRotation.Pitch, DesiredRotation.Yaw);
	//float ArcBaseAngle = FMath::RadiansToDegrees(FMath::Atan2(DesiredRotation.Pitch - (CurrentRotation.Pitch + Radius), DesiredRotation.Yaw));
	// FMath::RadiansToDegrees(acosf(FVector2D::DotProduct(FVector2D(YawDifference, PitchDifference), FVector2D(0, PitchDifference)))) * 2
	//float ArcLength = 2 * PI * FMath::Sqrt(PitchDifference * PitchDifference + YawDifference * YawDifference) * (ArcBaseAngle / 360);

	const FCollisionQueryParams Response = FCollisionQueryParams();
	FVector TraceStart = GetActorLocation();
	for (int i = 0; i < Arcs; i++)
	{
		for (int j = 0; j < RaysInArc; j++)
		{
			UE_LOG(LogTemp, Warning, TEXT("Arc: %d; PointOnArc: %d"), i, j);
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::FromInt(j));
			float NewPitch = Radius * i * (90 / (Arcs + 1)) * FMath::Sin((j + 1) * AngleBetweenRays + StartAngle);
			float NewYaw = Radius * i * (90 / (Arcs + 1)) * FMath::Cos((j + 1) * AngleBetweenRays + StartAngle);
			UE_LOG(LogTemp, Warning, TEXT("NewPoint Coordinates: %f:%f"), NewYaw, NewPitch);
			FVector TraceEnd = TraceStart + ((FRotator(NewPitch, NewYaw, CurrentRotation.Roll) + CurrentRotation).Vector() * Velocity * 5);
			FCollisionQueryParams TraceParams;
			TraceParams.AddIgnoredActor(this);
			if (!World->LineTraceSingleByChannel(LastHits[j], TraceStart, TraceEnd, ECollisionChannel::ECC_GameTraceChannel1, TraceParams))
			{
				DrawDebugLine(GetWorld(), TraceStart, TraceEnd, LastHits[j].bBlockingHit ? FColor::Red : FColor::Green, false, 2.0f, 0, 10.0f);
				for (int p = 0; p < 3; p++)
				{
					if (!World->LineTraceSingleByChannel(LastHits[j], TraceEnd, Path[0], ECollisionChannel::ECC_GameTraceChannel1, TraceParams)) {
						DrawDebugLine(GetWorld(), TraceStart, TraceEnd, LastHits[j].bBlockingHit ? FColor::Red : FColor::Green, false, 2.0f, 0, 10.0f);
						TraceEnd = TraceEnd + (TraceEnd - Path[0]).GetUnsafeNormal() * Velocity * SizeK;
						GoodGuesses.Add(TraceEnd);
						break;
					}
					else
					{
						TraceEnd += (TraceStart - TraceEnd).GetUnsafeNormal() * Velocity * SizeK;
					}
				}
			}
		}
	}
	/*FVector BestOption = GoodGuesses[0];
	for (int i = 0; i < GoodGuesses.Num(); i++)
	{
		if ((FVector::Distance(TraceStart, GoodGuesses[i]) + FVector::Distance(GoodGuesses[i], Path[0])) < (FVector::Distance(TraceStart, BestOption) + FVector::Distance(BestOption, Path[0])))
		{
			BestOption = GoodGuesses[i];
		}
	}
	Path.Insert(BestOption, 0);
	GetWorld()->SpawnActor<AActor>(MarkerClass, BestOption, FRotator());*/
}

