#include "MazeGraph.h"

const TArray<FIntPoint> FMazeGraph::Directions = { FIntPoint(0, -1), FIntPoint(-1, 0), FIntPoint(0, 1), FIntPoint(1, 0) };

FIntPoint FMazeGraph::Opposite(FIntPoint Direction)
{
	return FIntPoint(-Direction.X, -Direction.Y);
}

void FMazeGraph::Build(const FLayout& Layout)
{
	Width = Layout.Width;
	Height = Layout.Height;

	WalkableTiles.Init(false, Width * Height);
	for (int32 i = 0; i < Layout.Cells.Num(); i++)
	{
		WalkableTiles[i] = Layout.Cells[i].Type != ECellType::Wall;
	}

	Nodes.Empty();
	NodeByTile.Empty();

	for (int32 Y = 0; Y < Height; Y++)
	{
		for (int32 X = 0; X < Width; X++)
		{
			FIntPoint Tile(X, Y);
			if (IsWalkable(Tile) && IsNode(Tile))
			{
				FGraphNode Node;
				Node.Tile = Tile;
				NodeByTile.Add(Tile, Nodes.Add(Node));
			}
		}
	}

	for (int32 i = 0; i < Nodes.Num(); i++)
	{
		for (const FIntPoint& Direction : Directions)
		{
			int32 Weight = 0;
			int32 NextNode = FindNextNode(Nodes[i].Tile, Direction, Weight);
			if (NextNode != INDEX_NONE)
			{
				AddEdge(i, NextNode, Weight, Direction);
			}
		}
	}
}

int32 FMazeGraph::GetOrCreateNode(FIntPoint Tile)
{
	int32 ExistingNode = FindNode(Tile);
	if (ExistingNode != INDEX_NONE)
	{
		return ExistingNode;
	}

	if (!IsWalkable(Tile))
	{
		return INDEX_NONE;
	}

	FGraphNode Node;
	Node.Tile = Tile;
	int32 NewNode = Nodes.Add(Node);
	NodeByTile.Add(Tile, NewNode);

	for (const FIntPoint& Direction : Directions)
	{
		int32 Weight = 0;
		int32 NextNode = FindNextNode(Tile, Direction, Weight);
		if (NextNode == INDEX_NONE)
		{
			continue;
		}

		AddEdge(NewNode, NextNode, Weight, Direction);

		for (FGraphEdge& BackEdge : Nodes[NextNode].Edges)
		{
			if (BackEdge.Direction == Opposite(Direction))
			{
				BackEdge.ToNode = NewNode;
				BackEdge.Weight = Weight;
			}
		}
	}

	return NewNode;
}

void FMazeGraph::RemoveEdge(int32 NodeIndex, FIntPoint Direction)
{
	Nodes[NodeIndex].Edges.RemoveAll([Direction](const FGraphEdge& Edge)
	{
		return Edge.Direction == Direction;
	});
}

void FMazeGraph::CountFood(const TSet<FIntPoint>& FoodTiles)
{
	for (FGraphNode& Node : Nodes)
	{
		for (FGraphEdge& Edge : Node.Edges)
		{
			Edge.Food = 0;
			FIntPoint Tile = Node.Tile;
			for (int32 i = 0; i < Edge.Weight; i++)
			{
				Tile = GetNextTile(Tile, Edge.Direction);
				if (FoodTiles.Contains(Tile))
				{
					Edge.Food++;
				}
			}
		}
	}
}

int32 FMazeGraph::FindNode(FIntPoint Tile) const
{
	const int32* Index = NodeByTile.Find(Tile);
	if (Index == nullptr)
	{
		return INDEX_NONE;
	}
	return *Index;
}

bool FMazeGraph::IsWalkable(FIntPoint Tile) const
{
	if (Tile.X < 0 || Tile.Y < 0 || Tile.X >= Width || Tile.Y >= Height)
	{
		return false;
	}
	return WalkableTiles[Tile.Y * Width + Tile.X];
}

FIntPoint FMazeGraph::GetNextTile(FIntPoint Tile, FIntPoint Direction) const
{
	int32 X = (Tile.X + Direction.X + Width) % Width;
	int32 Y = Tile.Y + Direction.Y;
	return FIntPoint(X, Y);
}

bool FMazeGraph::IsNode(FIntPoint Tile) const
{
	TArray<FIntPoint> Exits = Directions.FilterByPredicate([this, Tile](const FIntPoint& Direction)
	{
		return IsWalkable(GetNextTile(Tile, Direction));
	});

	if (Exits.Num() != 2)
	{
		return true;
	}
	return Exits[1] != Opposite(Exits[0]);
}

int32 FMazeGraph::FindNextNode(FIntPoint From, FIntPoint Direction, int32& OutWeight) const
{
	OutWeight = 0;
	FIntPoint Current = From;

	while (true)
	{
		Current = GetNextTile(Current, Direction);
		if (!IsWalkable(Current))
		{
			return INDEX_NONE;
		}

		OutWeight++;

		int32 Node = FindNode(Current);
		if (Node != INDEX_NONE)
		{
			return Node;
		}
	}
}

void FMazeGraph::AddEdge(int32 FromNode, int32 ToNode, int32 Weight, FIntPoint Direction)
{
	FGraphEdge Edge;
	Edge.ToNode = ToNode;
	Edge.Weight = Weight;
	Edge.Direction = Direction;
	Nodes[FromNode].Edges.Add(Edge);
}
