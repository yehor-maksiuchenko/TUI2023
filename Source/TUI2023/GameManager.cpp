#include "GameManager.h"

UGameManager::UGameManager()
{

}

float UGameManager::FlightTime(float u, float angle, float y)
{
	float u_y = u * FMath::Sin(FMath::DegreesToRadians(angle));
	float D = u_y * u_y + 2 * G * y;
	float x1 = (-u_y + FMath::Sqrt(D)) / (-G);
	float x2 = (-u_y - FMath::Sqrt(D)) / (-G);
	return x2;
}

float UGameManager::MaxHeight(float u)
{
	float t = 0.5f * FlightTime(u, 90, 0);
	return (u * t - 0.5f * G * t * t);
}

float UGameManager::GetAngleOnThePlane(float x, float y)
{
	if (x == 0) {
		if (y < 0) return 270;
		if (y > 0) return 90;
	}
	float abs_x = abs(x);
	float result = FMath::Atan(y / abs_x) * 180 / PI;
	if (x < 0) result = 180 - result;
	return result;
}

void UGameManager::ParabolaPoint2D(float u, FVector2D& Position, float& angle1, float& angle2, float& time1, float& time2)
{
	float x = Position.X;
	float y = Position.Y;
	float x_0 = x;
	x = abs(x);
	float a = -G * 0.5 * FMath::Square(x / u);
	float b = x;
	float c = a - y;
	float D = pow(b, 2) - 4 * a * c;

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

void UGameManager::ParabolaPoint3D(float u, FVector& Position, float& horizontal_angle, float& vertical_angle1, float& vertical_angle2, float& time1, float& time2)
{
	float x = Position.X;
	float y = Position.Y;
	float z = Position.Z;

	FVector2D Position2D;

	Position2D.X = FMath::Sqrt(FMath::Square(x) + FMath::Square(y));
	Position2D.Y = z;
	horizontal_angle = GetAngleOnThePlane(x, y);

	ParabolaPoint2D(u, Position2D, vertical_angle1, vertical_angle2, time1, time2);
}

bool UGameManager::ParabolaParabola2D(UPARAM(ref, DisplayName = "Projectile Parameters") FProjectileParams& ProjectileParams, UPARAM(ref, DisplayName = "Target Parameters") FTargetParams& TargetParams, float Step, float& CollisionTime, FVector& CollisionPosition)
{
	// position => 0:-:1
	float ProjectilePitch = ProjectileParams.StartRotation.Pitch;
	float TargetPitch = TargetParams.StartRotation.Pitch;

	float x_0 = TargetParams.StartLocation.X - ProjectileParams.StartLocation.X;
	float y_0 = TargetParams.StartLocation.Z - ProjectileParams.StartLocation.Z;

	float u_x2 = TargetParams.Velocity * FMath::Cos(FMath::DegreesToRadians(TargetPitch));
	float u_y2 = TargetParams.Velocity * FMath::Sin(FMath::DegreesToRadians(TargetPitch));
	float Time = FlightTime(TargetParams.Velocity, TargetPitch, TargetParams.StartLocation.Z);
	float MaxH = MaxHeight(ProjectileParams.Velocity);
	ProjectileParams.DesiredRotation = FRotator(-90.f, 0, 0);
	CollisionTime = -1;
	ProjectileParams.WaitTime = -1;
	CollisionPosition.X = -1;
	CollisionPosition.Z = -1;

	for (float t = 0; t < Time; t += Step)
	{
		float s_x = u_x2 * t;
		float s_y = u_y2 * t - 0.5 * G * t * t;
		float x = x_0 + s_x;
		float y = y_0 + s_y;
		if (y > MaxH) continue;

		float angle1, TargetPitch1, time1, time2;
		FVector2D T{ x, y };
		ParabolaPoint2D(ProjectileParams.Velocity, T, angle1, TargetPitch1, time1, time2);
		if (time1 <= t && angle1 != -90)
		{
			float Rotation = abs(ProjectilePitch - angle1);
			float RotationTime = Rotation / ProjectileParams.RotationSpeed;
			ProjectileParams.WaitTime = t - time1;
			if (ProjectileParams.WaitTime >= RotationTime)
			{
				ProjectileParams.DesiredRotation = FRotator(angle1, 0, 0);
				CollisionTime = t;
				CollisionPosition.X = x;
				CollisionPosition.Z = y;
				break;
			}

		}
		if (time2 <= t && TargetPitch1 != -90)
		{
			float Rotation = abs(ProjectilePitch - TargetPitch1);
			float RotationTime = Rotation / ProjectileParams.RotationSpeed;
			ProjectileParams.WaitTime = t - time2;
			if (ProjectileParams.WaitTime >= RotationTime)
			{
				ProjectileParams.DesiredRotation = FRotator(TargetPitch1, 0, 0);
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

bool UGameManager::ParabolaParabola3D(UPARAM(ref, DisplayName = "Projectile Parameters") FProjectileParams& ProjectileParams, UPARAM(ref, DisplayName = "Target Parameters") FTargetParams& TargetParams, float Step, float& CollisionTime, FVector& CollisionPosition)
{
	float ProjectileYaw = ProjectileParams.StartRotation.Yaw;
	float ProjectilePitch = ProjectileParams.StartRotation.Pitch;
	float TargetYaw = TargetParams.StartRotation.Yaw;
	float TargetPitch = TargetParams.StartRotation.Pitch;

	float x_0 = TargetParams.StartLocation[0] - ProjectileParams.StartLocation[0];
	float y_0 = TargetParams.StartLocation[1] - ProjectileParams.StartLocation[1];
	float z_0 = TargetParams.StartLocation[2] - ProjectileParams.StartLocation[2];
	float u_x2 = TargetParams.Velocity * FMath::Cos(FMath::DegreesToRadians(TargetYaw)) * FMath::Cos(FMath::DegreesToRadians(TargetPitch));
	float u_y2 = TargetParams.Velocity * FMath::Cos(FMath::DegreesToRadians(90 - TargetYaw)) * FMath::Cos(FMath::DegreesToRadians(TargetPitch));
	float u_z2 = TargetParams.Velocity * FMath::Sin(FMath::DegreesToRadians(TargetPitch));
	float Time = FlightTime(TargetParams.Velocity, TargetPitch, TargetParams.StartLocation[2]);
	float MaxH = MaxHeight(ProjectileParams.Velocity);
	float result_vertical_angle = -90;
	CollisionTime = -1;
	ProjectileParams.WaitTime = -1;

	for (float t = Step; t < Time; t += Step) {

		float s_x = u_x2 * t;
		float s_y = u_y2 * t;
		float s_z = u_z2 * t - 0.5 * t * t * G;
		FVector CurrentPosition(x_0 + s_x, y_0 + s_y, z_0 + s_z);

		if (CurrentPosition[2] > MaxH) continue;
		float horizontal_angle, vertical_angle1, TargetPitch1, time1, time2;
		ParabolaPoint3D(ProjectileParams.Velocity, CurrentPosition, horizontal_angle, vertical_angle1, TargetPitch1, time1, time2);
		if (time1 <= t && time1 > 0) {
			ProjectileParams.WaitTime = t - time1;
			float RotationH = FMath::Min(abs(horizontal_angle - ProjectileYaw), (360 - horizontal_angle + ProjectileYaw));
			float RotationHTime = RotationH / ProjectileParams.LauncherRotationSpeedYaw;
			float RotationV = abs(ProjectilePitch - vertical_angle1);
			float RotationVTime = RotationV / ProjectileParams.LauncherRotationSpeedPitch;
			float RotationTime = FMath::Max(RotationHTime, RotationVTime);
			if (RotationTime <= ProjectileParams.WaitTime) {
				ProjectileParams.DesiredRotation = FRotator(vertical_angle1, horizontal_angle, 0);
				CollisionTime = t;
				CollisionPosition = CurrentPosition;
				break;
			}


		}
		if (time2 <= t && time2 > 0) {
			ProjectileParams.WaitTime = t - time2;
			float RotationH = FMath::Min(abs(horizontal_angle - ProjectileYaw), (360 - abs(horizontal_angle - ProjectileYaw)));
			float RotationHTime = RotationH / ProjectileParams.LauncherRotationSpeedYaw;
			float RotationV = abs(ProjectilePitch - TargetPitch1);
			float RotationVTime = RotationV / ProjectileParams.LauncherRotationSpeedPitch;
			float RotationTime = FMath::Max(RotationHTime, RotationVTime);
			if (RotationTime <= ProjectileParams.WaitTime) {
				ProjectileParams.DesiredRotation = FRotator(TargetPitch1, horizontal_angle, 0);
				CollisionTime = t;
				CollisionPosition = CurrentPosition;
				break;
			}
		}

	}

	if (CollisionTime > 0) return true;
	return false;
}

void UGameManager::SetPredictionResults(FRotator DesiredRotation, float WaitTime, FProjectileParams& ProjectileParams)
{
	ProjectileParams.DesiredRotation = DesiredRotation;
	ProjectileParams.WaitTime = WaitTime;
}

void UGameManager::ResetSimulation()
{
	if (Targets2D.Num() > 0)
	{
		for (int i = 0; i < Targets2D.Num(); i++) Targets2D[i]->Destroy();
		Targets2D.Empty();
	}
	if (Targets3D.Num() > 0)
	{
		for (int i = 0; i < Targets3D.Num(); i++) Targets3D[i]->Destroy();
		Targets3D.Empty();
	}
	if (Projectiles2D.Num() > 0)
	{
		for (int i = 0; i < Projectiles2D.Num(); i++) Projectiles2D[i]->Destroy();
		Projectiles2D.Empty();
	}
	if (Projectiles3D.Num() > 0)
	{
		for (int i = 0; i < Projectiles3D.Num(); i++) Projectiles3D[i]->Destroy();
		Projectiles3D.Empty();
	}
}

void UGameManager::LoadSimulation2D()
{
	if ((TargetsToLoad.Num() == ProjectilesToLoad.Num()) && TargetsToLoad.Num() > 0)
	{
		for (int i = 0; i < TargetsToLoad.Num(); i++)
		{
			ATarget2D* T = GetWorld()->SpawnActor<ATarget2D>(FVector(0, 0, 0), FRotator(0, 0, 0));
			T->InitializeTarget2D(TargetsToLoad[i], G, WorldSimulationSpeed);
			AProjectile2D* P = GetWorld()->SpawnActor<AProjectile2D>(FVector(0, 0, 0), FRotator(0, 0, 0));
			P->InitializeProjectile2D(ProjectilesToLoad[i], T, G, WorldSimulationSpeed);
			Targets2D.Add(T);
			Projectiles2D.Add(P);
		}
	}
}

void UGameManager::LoadSimulation3D()
{
	if ((TargetsToLoad.Num() == ProjectilesToLoad.Num()) && TargetsToLoad.Num() > 0)
	{
		for (int i = 0; i < TargetsToLoad.Num(); i++)
		{
			ATarget3D* T = GetWorld()->SpawnActor<ATarget3D>(FVector(0, 0, 0), FRotator(0, 0, 0));
			T->InitializeTarget3D(TargetsToLoad[i], G, WorldSimulationSpeed);
			AProjectile3D* P = GetWorld()->SpawnActor<AProjectile3D>(FVector(0, 0, 0), FRotator(0, 0, 0));
			P->InitializeProjectile3D(ProjectilesToLoad[i], T, G, WorldSimulationSpeed);
			Targets3D.Add(T);
			Projectiles3D.Add(P);
		}
	}
}
