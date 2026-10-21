// Fill out your copyright notice in the Description page of Project Settings.


#include "Dot.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

ADot::ADot()
{
	Points = 10;

	Collision->SetSphereRadius(10.f);
	Mesh->SetRelativeScale3D(FVector(0.2f));
}
