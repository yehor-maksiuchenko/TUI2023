#include "CameraPawn2D.h"

ACameraPawn2D::ACameraPawn2D()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	bUseControllerRotationYaw = false;

	MainComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	CamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));

	RootComponent = MainComp;
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.0f;
	CameraBoom->bDoCollisionTest = false;
	CamComp->SetupAttachment(CameraBoom);

	CameraBoom->SetRelativeRotation(FRotator(-CurCur, 0.f, 0.f));
}

void ACameraPawn2D::BeginPlay()
{
	Super::BeginPlay();

	TargetLocation = GetActorLocation();
	TargetZoom = 300.f;
}

void ACameraPawn2D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Bounds();

	FVector InterpolatedLocation = UKismetMathLibrary::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, MoveSpeed);
	SetActorLocation(InterpolatedLocation);

	const float InterpolatedZoom = UKismetMathLibrary::FInterpTo(CameraBoom->TargetArmLength, TargetZoom, DeltaTime, ZoomSpeed);
	CameraBoom->TargetArmLength = InterpolatedZoom;
}

void ACameraPawn2D::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveUp", this, &ACameraPawn2D::MoveUp);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACameraPawn2D::MoveRight);
	PlayerInputComponent->BindAxis("Zoom", this, &ACameraPawn2D::Zoom);
}

void ACameraPawn2D::MoveUp(float Value)
{
	if (Value)
	{
		TargetLocation += CameraBoom->GetUpVector() * Value * MoveSpeed;
	}
}

void ACameraPawn2D::MoveRight(float Value)
{
	if (Value)
	{
		TargetLocation += CameraBoom->GetRightVector() * Value * MoveSpeed;
	}
}

void ACameraPawn2D::Zoom(float Value)
{
	if (Value)
	{
		const float Zoom = Value * 100.0f;
		TargetZoom = FMath::Clamp(Zoom + TargetZoom, MinZoom, MaxZoom);
	}
}

void ACameraPawn2D::Bounds()
{
	if (TargetLocation.Y > MaxWidth) TargetLocation.Y = MaxWidth;
	if (TargetLocation.Y < MinWidth) TargetLocation.Y = MinWidth;

	if (TargetLocation.Z > MaxHeight) TargetLocation.Z = MaxHeight;
	if (TargetLocation.Z < MinHeight) TargetLocation.Z = MinHeight;
}

