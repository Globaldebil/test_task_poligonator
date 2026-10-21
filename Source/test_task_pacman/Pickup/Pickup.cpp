// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "../Game/PacmanGameMode.h"
#include "../Pacman/Pacman.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = Collision;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	Mesh->SetStaticMesh(SphereMesh.Object);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APickup::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnOverlap);
}

void APickup::SetMaterial(UMaterialInterface* Material)
{
	if (Material != nullptr)
	{
		Mesh->SetMaterial(0, Material);
	}
}

void APickup::OnEaten(APacmanGameMode* GameMode)
{
	GameMode->OnPickupEaten(this);
}

void APickup::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APacman* Pacman = Cast<APacman>(OtherActor);
	if (Pacman == nullptr)
	{
		return;
	}

	APacmanGameMode* GameMode = GetWorld()->GetAuthGameMode<APacmanGameMode>();
	if (GameMode != nullptr)
	{
		OnEaten(GameMode);
	}

	Destroy();
}
