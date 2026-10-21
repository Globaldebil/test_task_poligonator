// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Map.h"
#include "MazeGraph.h"
#include "PacmanMap.generated.h"

class AGhost;
class ADiikstraGhost;
class ABfsGhost;
class ADfsGhost;
class ABellmanFordGhost;
class UInstancedStaticMeshComponent;
class APickup;
class UMaterialInterface;

UCLASS()
class TEST_TASK_PACMAN_API APacmanMap : public AActor
{
	GENERATED_BODY()

public:
	APacmanMap();

	const FLayout& GetLayout() const { return Layout; }
	const FMazeGraph& GetGraph() const { return Graph; }
	float GetTileSize() const { return TileSize; }

	FVector TileToWorld(FIntPoint Tile) const;
	FIntPoint WorldToTile(const FVector& Location) const;

	void SpawnPickups();
	TSet<FIntPoint> GetPickupTiles() const;
	int32 GetPickupCount() const { return Pickups.Num(); }

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Map")
	float TileSize = 100.f;

	UPROPERTY(EditAnywhere, Category = "Map|Ghosts")
	TSubclassOf<ADiikstraGhost> DiikstraGhostClass;

	UPROPERTY(EditAnywhere, Category = "Map|Ghosts")
	TSubclassOf<ABfsGhost> BfsGhostClass;

	UPROPERTY(EditAnywhere, Category = "Map|Ghosts")
	TSubclassOf<ADfsGhost> DfsGhostClass;

	UPROPERTY(EditAnywhere, Category = "Map|Ghosts")
	TSubclassOf<ABellmanFordGhost> BellmanFordGhostClass;

	UPROPERTY(VisibleAnywhere, Category = "Map")
	TObjectPtr<UInstancedStaticMeshComponent> Walls;

	UPROPERTY(VisibleAnywhere, Category = "Map")
	TObjectPtr<UInstancedStaticMeshComponent> Door;

	UPROPERTY(EditAnywhere, Category = "Map|Pickups")
	TObjectPtr<UMaterialInterface> DotMaterial;

	UPROPERTY(EditAnywhere, Category = "Map|Pickups")
	TObjectPtr<UMaterialInterface> PillMaterial;

private:
	FVector TileToLocal(FIntPoint Tile) const;
	bool HasWalkableNeighbour(FIntPoint Tile) const;
	void BuildMeshes();
	void SpawnGhost(TSubclassOf<AGhost> GhostClass, FIntPoint Tile);
	void SpawnPickup(TSubclassOf<APickup> PickupClass, FIntPoint Tile, UMaterialInterface* Material);

	FLayout Layout;
	FMazeGraph Graph;

	UPROPERTY()
	TMap<FIntPoint, TObjectPtr<APickup>> Pickups;
};
