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

	const float InterpolatedZoom = UKismetMathLibrary::FInterpTo(CameraBoom->TargetArmLength, TargetZoom, DeltaTime, ZoomSpeed);
	CameraBoom->TargetArmLength = InterpolatedZoom;
}

void ACameraPawn2D::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

