#include "CameraPawn3D.h"

ACameraPawn::ACameraPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	bUseControllerRotationYaw = false;

	MainComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	CamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow Component"));
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

	RootComponent = MainComp;
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 2000.0f;
	CameraBoom->bDoCollisionTest = false;
	CamComp->SetupAttachment(CameraBoom);
	ArrowComp->SetupAttachment(CamComp);

	//CamComp->SetRelativeLocation(FVector(0, 0, 0));//LowestCameraHight));
	//CamComp->SetRelativeRotation(FRotator(-90, 0, 0));
}

void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();
	//SetActorLocationAndRotation(FVector(0, 0, LowestCameraHight), FRotator(-30, 0, 0));
	TargetLocation = GetActorLocation();
	TargetZoom = 3000.f;

	const FRotator Rotation = CameraBoom->GetRelativeRotation();
	TargetRotation = FRotator(Rotation.Pitch - 50, Rotation.Yaw, 0.0f);
}

void ACameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraBounds();

	FVector InterpolatedLocation = UKismetMathLibrary::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, MoveSpeed);
	SetActorLocation(InterpolatedLocation);

	const float InterpolatedZoom = UKismetMathLibrary::FInterpTo(CameraBoom->TargetArmLength, TargetZoom, DeltaTime, ZoomSpeed);
	CameraBoom->TargetArmLength = InterpolatedZoom;

	const FRotator InterpolatedRotation = UKismetMathLibrary::RInterpTo(CameraBoom->GetRelativeRotation(), TargetRotation, DeltaTime, RotateSpeed);
	CameraBoom->SetRelativeRotation(InterpolatedRotation);
}

void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//PlayerInputComponent->BindAxis("RotateHorizontal", this, &ACameraPawn::RotateHorizontal);
	//PlayerInputComponent->BindAxis("RotateVertical", this, &ACameraPawn::RotateVertical);
	PlayerInputComponent->BindAxis("MoveForward", this, &ACameraPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACameraPawn::MoveRight);
	PlayerInputComponent->BindAxis("Zoom", this, &ACameraPawn::Zoom);
	PlayerInputComponent->BindAxis("RotateRight", this, &ACameraPawn::RotateRight);
	PlayerInputComponent->BindAxis("RotateLeft", this, &ACameraPawn::RotateLeft);

	/*PlayerInputComponent->BindAction("ToRotate", EInputEvent::IE_Pressed, this, &ACameraPawn::EnableRotation);
	PlayerInputComponent->BindAction("ToRotate", EInputEvent::IE_Released, this, &ACameraPawn::DisableRotation);*/
}

//void ACameraPawn::RotateHorizontal(float Value)
//{
//	if (Value && bCanRotate)
//	{
//		TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, FRotator(0.f, Value, 0.f));
//	}
//}

//void ACameraPawn::RotateVertical(float Value)
//{
//	if (Value && bCanRotate)
//	{
//		TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, FRotator(Value, 0.f, 0.f));
//	}
//}

void ACameraPawn::MoveForward(float Value)
{
	if (Value)
	{
		TargetLocation += CameraBoom->GetForwardVector() * Value * MoveSpeed;
	}
}

void ACameraPawn::MoveRight(float Value)
{
	if (Value)
	{
		TargetLocation += CameraBoom->GetRightVector() * Value * MoveSpeed;
	}
}

void ACameraPawn::Zoom(float Value)
{
	if (Value)
	{
		const float Zoom = Value * 100.0f;
		TargetZoom = FMath::Clamp(Zoom + TargetZoom, MinZoom, MaxZoom);
	}
}

//void ACameraPawn::EnableRotation()
//{
//	bCanRotate = true;
//}

void ACameraPawn::RotateRight(float Value)
{
	if (Value)
	{
		TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, FRotator(0.f, -Value * 3, 0.f));
	}
}

void ACameraPawn::RotateLeft(float Value)
{
	if (Value)
	{
		TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, FRotator(0.f, Value * 3, 0.f));
	}
}

//void ACameraPawn::DisableRotation()
//{
//	bCanRotate = false;
//}

void ACameraPawn::CameraBounds()
{
	float NewPitch = TargetRotation.Pitch;
	/*if (NewPitch < (PitchMax * -1))
	{
		NewPitch = PitchMax * -1;
	}
	else if (TargetRotation.Pitch > (PitchMin * -1))
	{
		NewPitch = PitchMin * -1;
	}*/

	TargetRotation = FRotator(NewPitch, TargetRotation.Yaw, 0.0f);
	TargetLocation = FVector(TargetLocation.X, TargetLocation.Y, 0.0f);
}

