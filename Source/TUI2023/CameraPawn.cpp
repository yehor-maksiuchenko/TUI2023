#include "CameraPawn.h"

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
	CameraBoom->TargetArmLength = 200.0f;
	CamComp->SetupAttachment(CameraBoom);
	ArrowComp->SetupAttachment(CamComp);

	//CamComp->SetRelativeLocation(FVector(0, 0, 0));//LowestCameraHight));
	//CamComp->SetRelativeRotation(FRotator(-90, 0, 0));
}

void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();
	//SetActorLocationAndRotation(FVector(0, 0, LowestCameraHight), FRotator(-30, 0, 0));
	CamComp->SetRelativeLocationAndRotation(FVector(0, 0, LowestCameraHight), FRotator(-30, 0, 0));
}

void ACameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("TurnCamera", this, &ACameraPawn::TurnCamera);
	PlayerInputComponent->BindAxis("MoveForward", this, &ACameraPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACameraPawn::MoveRight);
	PlayerInputComponent->BindAxis("LiftCamera", this, &ACameraPawn::LiftCamera);
}

void ACameraPawn::TurnCamera(float Value)
{
	if (Value)
	{
		AddActorLocalRotation(FRotator(0, Value, 0));
	}
}

void ACameraPawn::MoveForward(float Value)
{
	if (Value)
	{
		AddActorWorldOffset(GetActorForwardVector() * GetWorld()->DeltaTimeSeconds * Value * 300);
	}
}

void ACameraPawn::MoveRight(float Value)
{
	if (Value)
	{
		AddActorWorldOffset(GetActorRightVector() * GetWorld()->DeltaTimeSeconds * Value * 300);
	}
}

void ACameraPawn::LiftCamera(float Value)
{
	if (Value)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(Value));
	}
}

