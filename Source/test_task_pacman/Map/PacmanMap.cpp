// Fill out your copyright notice in the Description page of Project Settings.


#include "PacmanMap.h"
#include "OriginalMap.h"
#include "../Ghost/BellmanFordGhost.h"
#include "../Ghost/BfsGhost.h"
#include "../Ghost/DfsGhost.h"
#include "../Ghost/DiikstraGhost.h"
#include "../Game/PacmanGameMode.h"
#include "../Pickup/Dot.h"
#include "../Pickup/PowerPill.h"
#include "Components/CapsuleComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

APacmanMap::APacmanMap()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));

	Walls = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Walls"));
	Walls->SetupAttachment(RootComponent);
	Walls->SetStaticMesh(CubeMesh.Object);

	Door = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(RootComponent);
	Door->SetStaticMesh(CubeMesh.Object);
}

void APacmanMap::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	Layout = FOriginalMap::CreateLayout();
	BuildMeshes();
}

void APacmanMap::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Layout = FOriginalMap::CreateLayout();
	Graph.Build(Layout);
}

void APacmanMap::BeginPlay()
{
	Super::BeginPlay();

	SpawnGhost(DiikstraGhostClass, Layout.DiicstraStart);
	SpawnGhost(BfsGhostClass, Layout.BfsStart);
	SpawnGhost(DfsGhostClass, Layout.DfsStart);
	SpawnGhost(BellmanFordGhostClass, Layout.BellmanFordStart);
	SpawnPickups();
}

FVector APacmanMap::TileToWorld(FIntPoint Tile) const
{
	return GetActorTransform().TransformPosition(TileToLocal(Tile));
}

FIntPoint APacmanMap::WorldToTile(const FVector& Location) const
{
	FVector LocalLocation = GetActorTransform().InverseTransformPosition(Location);
	int32 X = FMath::FloorToInt(LocalLocation.Y / TileSize + Layout.Width / 2.f);
	int32 Y = FMath::FloorToInt(Layout.Height / 2.f - LocalLocation.X / TileSize);
	return FIntPoint(X, Y);
}

FVector APacmanMap::TileToLocal(FIntPoint Tile) const
{
	float X = (Layout.Height / 2.f - Tile.Y - 0.5f) * TileSize;
	float Y = (Tile.X - Layout.Width / 2.f + 0.5f) * TileSize;
	return FVector(X, Y, 0.f);
}

bool APacmanMap::HasWalkableNeighbour(FIntPoint Tile) const
{
	for (int32 OffsetY = -1; OffsetY <= 1; OffsetY++)
	{
		for (int32 OffsetX = -1; OffsetX <= 1; OffsetX++)
		{
			FIntPoint Neighbour(Tile.X + OffsetX, Tile.Y + OffsetY);
			if (Layout.IsInside(Neighbour) && Layout.At(Neighbour).Type != ECellType::Wall)
			{
				return true;
			}
		}
	}
	return false;
}

void APacmanMap::BuildMeshes()
{
	Walls->ClearInstances();
	Door->ClearInstances();

	float Scale = TileSize / 100.f;

	for (int32 Y = 0; Y < Layout.Height; Y++)
	{
		for (int32 X = 0; X < Layout.Width; X++)
		{
			FIntPoint Tile(X, Y);
			const FCell& Cell = Layout.At(Tile);
			FVector Location = TileToLocal(Tile) + FVector(0.f, 0.f, TileSize / 2.f);

			if (Cell.Type == ECellType::Wall && HasWalkableNeighbour(Tile))
			{
				Walls->AddInstance(FTransform(FRotator::ZeroRotator, Location, FVector(Scale)));
			}
			if (Cell.Type == ECellType::Invisible_Wall)
			{
				Door->AddInstance(FTransform(FRotator::ZeroRotator, Location, FVector(0.1f * Scale, Scale, Scale)));
			}
		}
	}
}

void APacmanMap::SpawnGhost(TSubclassOf<AGhost> GhostClass, FIntPoint Tile)
{
	if (GhostClass == nullptr)
	{
		return;
	}

	FVector Location = TileToWorld(Tile);
	Location.Z += GhostClass->GetDefaultObject<AGhost>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<AGhost>(GhostClass, Location, FRotator::ZeroRotator, SpawnParams);
}

void APacmanMap::SpawnPickups()
{
	for (const TPair<FIntPoint, TObjectPtr<APickup>>& Pair : Pickups)
	{
		if (IsValid(Pair.Value))
		{
			Pair.Value->Destroy();
		}
	}
	Pickups.Empty();

	for (int32 Y = 0; Y < Layout.Height; Y++)
	{
		for (int32 X = 0; X < Layout.Width; X++)
		{
			FIntPoint Tile(X, Y);
			ECellObjects Type = Layout.At(Tile).Pickup;

			if (Type == ECellObjects::Score)
			{
				SpawnPickup(ADot::StaticClass(), Tile, DotMaterial);
			}
			else if (Type == ECellObjects::Pill)
			{
				SpawnPickup(APowerPill::StaticClass(), Tile, PillMaterial);
			}
		}
	}

	APacmanGameMode* GameMode = GetWorld()->GetAuthGameMode<APacmanGameMode>();
	if (GameMode != nullptr)
	{
		GameMode->SetTotalPickups(Pickups.Num());
	}
}

void APacmanMap::SpawnPickup(TSubclassOf<APickup> PickupClass, FIntPoint Tile, UMaterialInterface* Material)
{
	FVector Location = TileToWorld(Tile) + FVector(0.f, 0.f, TileSize / 2.f);
	APickup* Pickup = GetWorld()->SpawnActor<APickup>(PickupClass, Location, FRotator::ZeroRotator);
	Pickup->SetMaterial(Material);
	Pickups.Add(Tile, Pickup);
}

TSet<FIntPoint> APacmanMap::GetPickupTiles() const
{
	TSet<FIntPoint> Tiles;
	for (const TPair<FIntPoint, TObjectPtr<APickup>>& Pair : Pickups)
	{
		if (IsValid(Pair.Value))
		{
			Tiles.Add(Pair.Key);
		}
	}
	return Tiles;
}
