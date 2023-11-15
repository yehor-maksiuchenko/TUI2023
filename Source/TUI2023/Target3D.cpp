// Fill out your copyright notice in the Description page of Project Settings.


#include "Target3D.h"

// Sets default values
ATarget3D::ATarget3D()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATarget3D::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATarget3D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

