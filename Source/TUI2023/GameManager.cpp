#include "GameManager.h"

AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AGameManager::FlightTime(float u, float angle, float y)
{
	float u_y = u * FMath::Sin(FMath::DegreesToRadians(angle));
	float D = u_y * u_y + 2 * G * y;
	float x1 = (-u_y + FMath::Sqrt(D)) / (-G);
	float x2 = (-u_y - FMath::Sqrt(D)) / (-G);
	return x2;
}

float AGameManager::MaxHeight(float u)
{
	float t = 0.5f * FlightTime(u, 90, 0);
	return (u * t - 0.5f * G * t * t);
}

void AGameManager::ParabolaPoint2D(float u, float x, float y, float& angle1, float& angle2, float& time1, float& time2)
{
	float x_0 = x;
	x = FMath::Abs(x);
	float a = -G * 0.5 * FMath::Square(x / u);
	float b = x;
	float c = a - y;
	float D = FMath::Square(b) - 4 * a * c;

	if (D > 0) {
		float t1 = (-1 * b + FMath::Sqrt(D)) / (2 * a);
		float t2 = (-1 * b - FMath::Sqrt(D)) / (2 * a);

		angle1 = FMath::Atan(t1) * 180 / PI;
		angle2 = FMath::Atan(t2) * 180 / PI;

		time1 = x / (u * FMath::Cos(FMath::DegreesToRadians(angle1)));
		time2 = x / (u * FMath::Cos(FMath::DegreesToRadians(angle2)));

	}
	else if (D == 0) {

		float t1 = (-1 * b + FMath::Sqrt(D)) / (2 * a);
		float t2 = (-1 * b - FMath::Sqrt(D)) / (2 * a);

		angle1 = FMath::Atan(t1) * 180 / PI;
		angle2 = FMath::Atan(t2) * 180 / PI;

		time1 = x / (u * FMath::Cos(FMath::DegreesToRadians(angle1)));
		time2 = x / (u * FMath::Cos(FMath::DegreesToRadians(angle2)));
	}
	else {

		angle1 = -90;
		angle2 = -90;
		time1 = -1;
		time2 = -1;
	}
	if (x_0 < 0) {
		angle1 = 180 - angle1;
		angle2 = 180 - angle2;
	}
}

void AGameManager::ParabolaPoint3D(float u, FVector Position, float& horizontal_angle, float& vertical_angle1, float& vertical_angle2, float& time1, float& time2)
{
	float x = Position.X;
	float y = Position.Y;
	float z = Position.Z;

	TArray <float> TargetLocationD(0, 0);

	TargetLocationD[0] = FMath::Sqrt(FMath::Square(x) + FMath::Square(y));
	TargetLocationD[1] = z;
	horizontal_angle = FMath::Acos((x / TargetLocationD[0])) * 180 / PI;

	ParabolaPoint2D(u, TargetLocationD[0], TargetLocationD[1], vertical_angle1, vertical_angle2, time1, time2);
}

bool AGameManager::ParabolaParabola2D(FVector ProjectilePosition, float ProjectileRotation, float ProjectileVelocity, float RotationSpeed, FVector TargetPosition, float TargetRotation, float TargetVelocity, float Step, float& ResultAngle, float& WaitTime, float& CollisionTime, FVector& CollisionPosition)
{
	// position => 0:-:1
	float x_0 = TargetPosition.X - ProjectilePosition.X;
	float y_0 = TargetPosition.Z - ProjectilePosition.Z;

	float u_x2 = TargetVelocity * FMath::Cos(FMath::DegreesToRadians(TargetRotation));
	float u_y2 = TargetVelocity * FMath::Sin(FMath::DegreesToRadians(TargetRotation));
	float Time = FlightTime(TargetVelocity, TargetRotation, TargetPosition.Z);
	float MaxH = MaxHeight(ProjectileVelocity);
	ResultAngle = -90;
	CollisionTime = -1;
	WaitTime = -1;
	CollisionPosition.X = -1;
	CollisionPosition.Z = -1;

	for (float t = 0; t < Time; t += Step)
	{
		float s_x = u_x2 * t;
		float s_y = u_y2 * t - 0.5 * G * t * t;
		float x = x_0 + s_x;
		float y = y_0 + s_y;
		if (y > MaxH) continue;

		float angle1, TargetRotation, time1, time2;
		ParabolaPoint2D(ProjectileVelocity, x, y, angle1, TargetRotation, time1, time2);
		if (time1 <= t && angle1 != -90)
		{
			float Rotation = abs(ProjectileRotation - angle1);
			float RotationTime = Rotation / RotationSpeed;
			WaitTime = t - time1;
			if (WaitTime >= RotationTime)
			{
				ResultAngle = angle1;
				CollisionTime = t;
				CollisionPosition.X = x;
				CollisionPosition.Z = y;
				break;
			}

		}
		if (time2 <= t && TargetRotation != -90)
		{
			float Rotation = abs(ProjectileRotation - TargetRotation);
			float RotationTime = Rotation / RotationSpeed;
			WaitTime = t - time2;
			if (WaitTime >= RotationTime)
			{
				ResultAngle = TargetRotation;
				CollisionTime = t;
				CollisionPosition.X = x;
				CollisionPosition.Z = y;
				break;
			}
		}

	}
	if (CollisionTime > 0) return true;
	return false;
}

bool AGameManager::ParabolaParabola3D(FVector ProjectileLocation, float ProjectileYaw, float ProjectilePitch, float YawRotationSpeed, float PitchRotationSpeed, float ProjectileVelocity, FVector TargetLocation, float TargetYaw, float TargetPitch, float TargetVelocity, float& ResultYaw, float& ResultPitch, float Step, float& CollisionTime, float& WaitTime)
{
	float x_0 = TargetLocation[0] - ProjectileLocation[0];
	float y_0 = TargetLocation[1] - ProjectileLocation[1];
	float z_0 = TargetLocation[2] - ProjectileLocation[2];
	float u_x2 = TargetVelocity * FMath::Cos(FMath::DegreesToRadians(TargetYaw)) * FMath::Cos(FMath::DegreesToRadians(TargetPitch));
	float u_y2 = TargetVelocity * FMath::Cos(FMath::DegreesToRadians(90 - TargetYaw)) * FMath::Cos(FMath::DegreesToRadians(TargetPitch));
	float u_z2 = TargetVelocity * FMath::Sin(FMath::DegreesToRadians(TargetPitch));
	float Time = FlightTime(TargetVelocity, TargetPitch, TargetLocation[2]);
	float MaxH = MaxHeight(ProjectileVelocity);
	float result_vertical_angle = -90;
	CollisionTime = -1;
	WaitTime = -1;

	for (float t = Step; t < Time; t += Step) {

		float s_x = u_x2 * t;
		float s_y = u_y2 * t;
		float s_z = u_z2 * t - 0.5 * t * t * G;
		FVector CurrentPosition (x_0 + s_x, y_0 + s_y, z_0 + s_z);
		//CurrentPosition[0] = x_0 + s_x;
		//CurrentPosition[1] = y_0 + s_y;
		//CurrentPosition[2] = z_0 + s_z;

		if (CurrentPosition[2] > MaxH) continue;
		float horizontal_angle, vertical_angle1, TargetPitch, time1, time2;
		ParabolaPoint3D(ProjectileVelocity, CurrentPosition, horizontal_angle, vertical_angle1, TargetPitch, time1, time2);
		if (time1 <= t && time1 > 0) {
			WaitTime = t - time1;
			float RotationH = FMath::Min(abs(horizontal_angle - ProjectileYaw), (360 - horizontal_angle + ProjectileYaw));
			float RotationHTime = RotationH / YawRotationSpeed;
			float RotationV = abs(ProjectilePitch - vertical_angle1);
			float RotationVTime = RotationV / PitchRotationSpeed;
			float RotationTime = FMath::Max(RotationHTime, RotationVTime);
			if (RotationTime <= WaitTime) {
				ResultYaw = horizontal_angle;
				ResultPitch = vertical_angle1;
				CollisionTime = t;
				break;
			}


		}
		if (time2 <= t && time2 > 0) {
			WaitTime = t - time2;
			float RotationH = FMath::Min(abs(horizontal_angle - ProjectileYaw), (360 - abs(horizontal_angle - ProjectileYaw)));
			float RotationHTime = RotationH / YawRotationSpeed;
			float RotationV = abs(ProjectilePitch - TargetPitch);
			float RotationVTime = RotationV / PitchRotationSpeed;
			float RotationTime = FMath::Max(RotationHTime, RotationVTime);
			if (RotationTime <= WaitTime) {
				ResultYaw = horizontal_angle;
				ResultPitch = TargetPitch;
				CollisionTime = t;
				break;
			}
		}

	}

	if (CollisionTime > 0) return true;
	return false;
}
