#pragma once

#include "CoreMinimal.h"

enum class ECellType : uint8
{
	Wall,
	EmptyCell,
	Respawn,
	Invisible_Wall
};

enum class ECellObjects : uint8
{
	None,
	Score,
	Pill
};

struct FCell
{
	ECellType Type = ECellType::Wall;
	ECellObjects Pickup = ECellObjects::None;
};

struct FLayout
{
	int32 Width = 0;
	int32 Height = 0;
	TArray<FCell> Cells;

	FIntPoint PacmanStart;
	FIntPoint DiicstraStart;
	FIntPoint BfsStart;
	FIntPoint DfsStart;
	FIntPoint BellmanFordStart;
	FIntPoint InvisibleWallNode; 
	FIntPoint HouseCenter;

	bool IsInside(FIntPoint C) const { return C.X >= 0 && C.Y >= 0 && C.X < Width && C.Y < Height; }
	const FCell& At(FIntPoint C) const { return Cells[C.Y * Width + C.X]; }
	bool IsWalkable(FIntPoint C) const { return IsInside(C) && At(C).Type == ECellType::EmptyCell; }
};