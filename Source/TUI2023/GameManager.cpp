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

void UGameManager::LinePoint2D(float u, FVector2D Position, float& result_angle, float& time)
{
	result_angle = GetAngleOnThePlane(Position[0], Position[1]);
	float S = FMath::Sqrt(Position[0] * Position[0] + Position[1] * Position[1]);
	time = S / u;
}

void UGameManager::LinePoint3D(float v, FVector Position, float& horizontal_angle, float& vertical_angle, float& time)
{
	horizontal_angle = GetAngleOnThePlane(Position.X, Position.Y);
	vertical_angle = GetAngleOnThePlane(FMath::Sqrt(Position.X * Position.X + Position.Y * Position.Y), Position.Z);

	float S = FMath::Sqrt(Position[0] * Position[0] + Position[1] * Position[1] + Position[2] * Position[2]);
	double t_a = v / 1;
	double S_a = v / 2 * t_a;
	S -= S_a;
	time = (S / v) + t_a;
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

bool UGameManager::LineParabola2D(FProjectileParams Projectile, FTargetParams Target, float Step, float& CollisionTime, FVector& CollisionPosition)
{
	float x_0 = Target.StartLocation.X - Projectile.StartLocation.X;
	float y_0 = Target.StartLocation.Z - Projectile.StartLocation.Z;

	float u_x2 = Target.Velocity * FMath::Cos(FMath::DegreesToRadians(Target.StartRotation.Pitch));
	float u_y2 = Target.Velocity * FMath::Sin(FMath::DegreesToRadians(Target.StartRotation.Pitch));
	FPredictProjectilePathParams Params;
	Params.StartLocation = Projectile.StartLocation;
	Params.OverrideGravityZ = 0;
	Params.bTraceComplex = true;
	Params.bTraceWithCollision = true;
	Params.LaunchVelocity = Projectile.StartRotation.Vector() * Projectile.Velocity;
	Params.MaxSimTime = FlightTime(Target.Velocity, Target.StartRotation.Pitch, Target.StartLocation.Z);
	Params.SimFrequency = 15;

	FPredictProjectilePathResult Result;
	float Time;
	if (UGameplayStatics::PredictProjectilePath(this, Params, Result))
	{
		Time = Result.HitResult.Time * Params.MaxSimTime;//TimeOfFlight(Target.Velocity, Target.StartRotation.Pitch, Target.StartLocation[1]);//Get the time of colission of a Balistic Missile with the ground
	}
	
	Projectile.DesiredRotation.Pitch = -90;
	CollisionTime = -1;
	Projectile.WaitTime = -1;
	CollisionPosition.X = -1;
	CollisionPosition.Z = -1;



	for (float t = 0; t < Time; t += Step) {
		float s_x = u_x2 * t;
		float s_y = u_y2 * t - 0.5 * g * t * t;
		float x = x_0 + s_x;
		float y = y_0 + s_y;


		float angle1 = 0, time1 = -1;
		FVector2D CurrentPosition = FVector2D(x, y);
		LinePoint2D(Projectile.Velocity, CurrentPosition, angle1, time1);

		if (CollisionTime < Time) continue; // check the collision with the suroundings for the projectile, fired at an angle = angle1, with g = 0
		if (time1 * Projectile.FuelExpense > Projectile.FuelTank) continue;
		if (time1 <= t && angle1 != -90) {
			float Rotation = abs(Projectile.StartRotation.Pitch - angle1);
			float RotationTime = Rotation / Projectile.RotationSpeed;
			Projectile.WaitTime = t - time1;
			if (Projectile.WaitTime >= RotationTime) {
				Projectile.DesiredRotation.Pitch = angle1;
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

bool UGameManager::ParabolaParabola3D(UPARAM(ref, DisplayName = "Projectile Parameters") FProjectileParams& ProjectileParams, UPARAM(ref, DisplayName = "Target Parameters") FTargetParams& TargetParams, TArray <TArray<situation_data>>& situations_matrix, float Step, float& CollisionTime, FVector& CollisionPosition)
{
	float ProjectileYaw = ProjectileParams.StartRotation.Yaw;
	float ProjectilePitch = ProjectileParams.StartRotation.Pitch;
	float TargetYaw = TargetParams.StartRotation.Yaw;
	float TargetPitch = TargetParams.StartRotation.Pitch;

	float x_0 = TargetParams.StartLocation.X - ProjectileParams.StartLocation.X;
	float y_0 = TargetParams.StartLocation.Y - ProjectileParams.StartLocation.Y;
	float z_0 = TargetParams.StartLocation.Z - ProjectileParams.StartLocation.Z;
	float u_x2 = TargetParams.Velocity * FMath::Cos(FMath::DegreesToRadians(TargetYaw)) * FMath::Cos(FMath::DegreesToRadians(TargetPitch));
	float u_y2 = TargetParams.Velocity * FMath::Cos(FMath::DegreesToRadians(90 - TargetYaw)) * FMath::Cos(FMath::DegreesToRadians(TargetPitch));
	float u_z2 = TargetParams.Velocity * FMath::Sin(FMath::DegreesToRadians(TargetPitch));
	float Time = FlightTime(TargetParams.Velocity, TargetPitch, TargetParams.StartLocation.Z);
	//float Time = UGameplayStatics::Pre
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

bool UGameManager::LineParabola3D(FProjectileParams Projectile, FTargetParams Target, float Step, float& CollisionTime)
{
	float x_0 = Target.StartLocation.X - Projectile.StartLocation.X;
	float y_0 = Target.StartLocation.Y - Projectile.StartLocation.Y;
	float z_0 = Target.StartLocation.Z - Projectile.StartLocation.Z;
	float u_x2 = Target.Velocity * FMath::Cos(FMath::DegreesToRadians(Target.StartRotation.Yaw)) * FMath::Cos(FMath::DegreesToRadians(Target.StartRotation.Pitch));
	float u_y2 = Target.Velocity * FMath::Cos(FMath::DegreesToRadians(90 - Target.StartRotation.Yaw)) * FMath::Cos(FMath::DegreesToRadians(Target.StartRotation.Pitch));
	float u_z2 = Target.Velocity * FMath::Sin(FMath::DegreesToRadians(Target.StartRotation.Pitch));
	FPredictProjectilePathParams Params;
	Params.StartLocation = Projectile.StartLocation;
	Params.OverrideGravityZ = 0;
	Params.bTraceComplex = true;
	Params.bTraceWithCollision = true;
	Params.LaunchVelocity = Projectile.StartRotation.Vector() * Projectile.Velocity;
	Params.MaxSimTime = 65;
	Params.SimFrequency = 15;

	FPredictProjectilePathResult Result;
	float Time;
	if (UGameplayStatics::PredictProjectilePath(this, Params, Result))
	{
		Time = Result.HitResult.Time * Params.MaxSimTime;
	}
	float result_vertical_angle = -90;
	CollisionTime = -1;
	Projectile.WaitTime = -1;

	for (float t = Step; t < Time; t += Step) {

		float s_x = u_x2 * t;
		float s_y = u_y2 * t;
		float s_z = u_z2 * t - 0.5 * t * t * G;
		FVector CurrentPosition = FVector(x_0 + s_x, y_0 + s_y, z_0 + s_z);

		float horizontal_angle, vertical_angle1, time1, time2;
		LinePoint3D(Projectile.Velocity, CurrentPosition, horizontal_angle, vertical_angle1, time1);
		//if (!UGameplayStatics::PredictProjectilePath()) continue; // check the collision with the suroundings for the projectile, fired at an angle = angle1, with g = 0
		if (time1 * Projectile.FuelExpense > Projectile.FuelTank) continue;

		if (time1 <= t && time1 > 0) {
			Projectile.WaitTime = t - time1;
			float RotationH = FMath::Min(abs(horizontal_angle - Projectile.StartRotation.Yaw), (360 - horizontal_angle + Projectile.StartRotation.Yaw));
			float RotationHTime = RotationH / Projectile.LauncherRotationSpeedYaw;
			float RotationV = abs(Projectile.StartRotation.Pitch - vertical_angle1);
			float RotationVTime = RotationV / Projectile.LauncherRotationSpeedPitch;
			float RotationTime = FMath::Max(RotationHTime, RotationVTime);
			if (RotationTime <= Projectile.WaitTime) {
				Projectile.DesiredRotation.Yaw = horizontal_angle;
				Projectile.DesiredRotation.Pitch = vertical_angle1;
				CollisionTime = t;
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
	if (Launchers.Num() > 0)
	{
		for (int i = 0; i < Launchers.Num(); i++) Launchers[i]->Destroy();
		Launchers.Empty();
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

struct object {
	float x = 0;
	float y = 0;
	float z = 0;

	float u = 0;
	float a = 0;

	bool IsBalistic = false;

	float horizontal_angle;
	float vertical_angle;
	float c_v = 1;
	float c_h = 1;

	float fuel_amount;
	float fuel_price;
};


void UGameManager::CreatingSituation3D(TArray <TArray<situation_data>>& situations_matrix) {
	int target_amount = TargetsToLoad.Num();
	int projectile_amount = ProjectilesToLoad.Num();
	for (int i = 0; i < target_amount; i++) {
		for (int j = 0; j < projectile_amount; j++) {
			FVector Position1 = ProjectilesToLoad[j].StartLocation;
			FVector Position2 = TargetsToLoad[i].StartLocation;
			bool f;
			if (!TargetsToLoad[i].isBallistic) {
				situations_matrix[i][j].IsPossible = false;
				continue;
			}
			if (ProjectilesToLoad[j].isBallistic) {
				f = ParabolaParabola3D(ProjectilesToLoad[j], TargetsToLoad[i], 0.01, situations_matrix[i][j].CollisionTime, situations_matrix[i][j].CollisionPosition);
				situations_matrix[i][j].IsPossible = f;
			}
			else {
				f = LineParabola3D(ProjectilesToLoad[j], TargetsToLoad[i], 0.01, situations_matrix[i][j].CollisionTime);
				situations_matrix[i][j].IsPossible = f;

			}
		}
	}
}

void UGameManager::CreatingSituation2D(vector <object>& projectiles, vector <object>& targets, vector <vector<situation_data>>& situations_matrix) {
	int target_amount = targets.size();
	int projectile_amount = projectiles.size();
	for (int i = 0; i < target_amount; i++) {
		for (int j = 0; j < projectile_amount; j++) {
			vector <float> Position1(2);
			Position1[0] = projectiles[j].x;
			Position1[1] = projectiles[j].z;
			//Position1[2] = projectiles[j].z;
			vector <float> Position2(2);
			Position2[0] = targets[i].x;
			Position2[1] = targets[i].z;
			//Position2[2] = targets[i].z;
			bool f;
			if (targets[i].IsBalistic == false) {
				situations_matrix[i][j].IsPossible = false;
				continue;
			}
			if (projectiles[j].IsBalistic) {
				f = ParabolaParabola2D(projectiles[j].u, targets[i].u, projectiles[j].vertical_angle, targets[i].vertical_angle, projectiles[j].c_v, Position1, Position2, situations_matrix[i][j].vertical_angle, 0.01, situations_matrix[i][j].CollisionTime, situations_matrix[i][j].WaitTime);
				situations_matrix[i][j].IsPossible = f;
			}
			else {
				vector <float> CollisionPosition(2);


				f = LineParabola2D(projectiles[j].u, projectiles[j].a, targets[i].u, projectiles[j].vertical_angle, targets[i].vertical_angle, projectiles[j].c_v, Position1, Position2, situations_matrix[i][j].vertical_angle, 0.01, situations_matrix[i][j].CollisionTime, situations_matrix[i][j].WaitTime, CollisionPosition, projectiles[j].fuel_amount, projectiles[j].fuel_price);
				situations_matrix[i][j].IsPossible = f;
				situations_matrix[i][j].CollisionPosition_x = CollisionPosition[0];
				situations_matrix[i][j].CollisionPosition_y = CollisionPosition[1];
			}
			//cout << f << ' ';
		}
		cout << endl;
	}
}



void UGameManager::ProcessingSituation(vector <object>& projectiles, vector <object>& targets, vector <vector<situation_data>>& situations_matrix, vector <int>& linkage) {

	int target_amount = targets.size();
	int projectile_amount = projectiles.size();
	//vector <int> linkage(0);
	vector <bool> ocupied_projectiles(projectile_amount, false);
	//vector < vector<situation_data>> situations_matrix;
	situations_matrix.resize(target_amount);
	for (int i = 0; i < target_amount; i++) {
		situations_matrix[i].resize(projectile_amount);
	}
	//CreatingSituation3D(projectiles, targets, situations_matrix);
	CreatingSituation2D(projectiles, targets, situations_matrix);
	for (int i = 0; i < target_amount; i++) {
		for (int j = 0; j < projectile_amount; j++) cout << situations_matrix[i][j].IsPossible << ' ';
		cout << endl;
	}

	//CreatingSituation()

	int max_possible = min(projectile_amount, target_amount);
	int amount = 1;
	int max_current = 0;
	vector<int> result;
	vector<vector<int> > matrix;


	matrix.resize(target_amount);
	cout << endl;

	for (int i = 0; i < target_amount; i++) {
		int a = 0, b = 0;
		for (int j = 0; j < projectile_amount; j++) {
			cout << situations_matrix[i][j].IsPossible << ' ';
			if (situations_matrix[i][j].IsPossible == true) {
				matrix[i].push_back(j);
				a++;
			}
			else {
				b++;
			}


		}
		if (b == projectile_amount) max_possible--;
		if (a != 0) amount *= a;
		cout << endl;

	}

	//cout << endl;

	for (int i = 0; i < target_amount; i++) {

		for (int j = 0; j < matrix[i].size(); j++) {
			//cout << matrix[i][j] << " ";
		}

		//cout << endl;

	}
	//cout << amount << endl << endl;
	for (int i = 0; i < amount; i++) {
		int  sum = 0;
		int  t1 = i, t2 = amount;
		vector<int > ans;
		set<int > arr;
		for (int j = 0; j < target_amount; j++) {

			if (matrix[j].size() != 0) {
				t2 /= matrix[j].size();
				int  k = t1 / t2;
				t1 = t1 - k * t2;

				ans.push_back(matrix[j][k]);
				arr.insert(matrix[j][k]);
			}
			else {
				ans.push_back(-1);
				arr.insert(-1);
			}



			t2 /= matrix[j].size();
			int  k = t1 / t2;
			t1 = t1 - k * t2;

			ans.push_back(matrix[j][k]);
			arr.insert(matrix[j][k]);


		}
		if (arr.size() > max_current) {
			max_current = arr.size();
			result = ans;
		}

	}

	vector<int > output;
	for (int i = 0; i < result.size(); i++) {
		if (InArr(output, result[i])) {
			output.push_back(-1);
		}
		else {
			output.push_back(result[i]);
		}

	}

	linkage = output;
	for (int i = 0; i < target_amount; i++) {
		if (linkage[i] > -1) ocupied_projectiles[linkage[i]] = true;
	}
	cout << endl << endl;
	for (int i = 0; i < target_amount; i++) {
		if (linkage[i] == -1 && targets[i].IsBalistic == false) {
			for (int j = 0; j < projectile_amount; j++) {
				if (ocupied_projectiles[j] == false && projectiles[j].IsBalistic == false) {
					linkage[i] = j;
					ocupied_projectiles[j] = true;
				}
			}
		}
	}
}
