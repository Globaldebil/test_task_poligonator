#pragma once

#include "CoreMinimal.h"
#include "Map.h"

struct FGraphEdge
{
	int32 ToNode = INDEX_NONE;
	int32 Weight = 0;
	int32 Food = 0;
	FIntPoint Direction;
};

struct FGraphNode
{
	FIntPoint Tile;
	TArray<FGraphEdge> Edges;
};

class FMazeGraph
{
public:
	static const TArray<FIntPoint> Directions;

	static FIntPoint Opposite(FIntPoint Direction);

	void Build(const FLayout& Layout);

	int32 GetOrCreateNode(FIntPoint Tile);
	void RemoveEdge(int32 NodeIndex, FIntPoint Direction);
	void CountFood(const TSet<FIntPoint>& FoodTiles);

	int32 FindNode(FIntPoint Tile) const;
	int32 GetNodeCount() const { return Nodes.Num(); }
	const FGraphNode& GetNode(int32 Index) const { return Nodes[Index]; }

	bool IsWalkable(FIntPoint Tile) const;
	FIntPoint GetNextTile(FIntPoint Tile, FIntPoint Direction) const;

private:
	bool IsNode(FIntPoint Tile) const;
	int32 FindNextNode(FIntPoint From, FIntPoint Direction, int32& OutWeight) const;
	void AddEdge(int32 FromNode, int32 ToNode, int32 Weight, FIntPoint Direction);

	int32 Width = 0;
	int32 Height = 0;
	TArray<bool> WalkableTiles;
	TArray<FGraphNode> Nodes;
	TMap<FIntPoint, int32> NodeByTile;
};
