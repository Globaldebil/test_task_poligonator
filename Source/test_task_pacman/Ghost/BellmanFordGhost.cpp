// Fill out your copyright notice in the Description page of Project Settings.


#include "BellmanFordGhost.h"
#include "../Map/MazeGraph.h"
#include "Containers/Queue.h"

ABellmanFordGhost::ABellmanFordGhost()
{
	PrimaryActorTick.bCanEverTick = true;

	BodyColor = FLinearColor(FColor(139, 69, 19));
}

TArray<int32> ABellmanFordGhost::FindPath(const FMazeGraph& Graph, int32 From, int32 To) const
{
	int32 NodeCount = Graph.GetNodeCount();
	TArray<FBellmanFordEdge> Edges = GetForwardEdges(Graph, From);

	TArray<int32> Distances;
	Distances.Init(MAX_int32, NodeCount);
	Distances[From] = 0;

	TMap<int32, int32> Parents;
	Parents.Add(From, From);

	for (int32 i = 0; i < NodeCount - 1; i++)
	{
		bool bChanged = false;
		for (const FBellmanFordEdge& Edge : Edges)
		{
			if (Distances[Edge.From] == MAX_int32)
			{
				continue;
			}

			int32 NewDistance = Distances[Edge.From] + Edge.Weight;
			if (NewDistance < Distances[Edge.To])
			{
				Distances[Edge.To] = NewDistance;
				Parents.Add(Edge.To, Edge.From);
				bChanged = true;
			}
		}

		if (!bChanged)
		{
			break;
		}
	}

	if (HasNegativeCycle(Edges, Distances))
	{
		return TArray<int32>();
	}

	return BuildPath(Parents, From, To);
}

TArray<int32> ABellmanFordGhost::GetDepths(const FMazeGraph& Graph, int32 From) const
{
	TArray<int32> Depths;
	Depths.Init(INDEX_NONE, Graph.GetNodeCount());
	Depths[From] = 0;

	TQueue<int32> Queue;
	Queue.Enqueue(From);

	while (!Queue.IsEmpty())
	{
		int32 Current;
		Queue.Dequeue(Current);

		for (const FGraphEdge& Edge : Graph.GetNode(Current).Edges)
		{
			if (Depths[Edge.ToNode] == INDEX_NONE)
			{
				Depths[Edge.ToNode] = Depths[Current] + 1;
				Queue.Enqueue(Edge.ToNode);
			}
		}
	}

	return Depths;
}

TArray<FBellmanFordEdge> ABellmanFordGhost::GetForwardEdges(const FMazeGraph& Graph, int32 From) const
{
	TArray<int32> Depths = GetDepths(Graph, From);
	TArray<FBellmanFordEdge> Edges;

	for (int32 Node = 0; Node < Graph.GetNodeCount(); Node++)
	{
		if (Depths[Node] == INDEX_NONE)
		{
			continue;
		}

		for (const FGraphEdge& Edge : Graph.GetNode(Node).Edges)
		{
			if (Depths[Edge.ToNode] == Depths[Node] + 1)
			{
				int32 Weight = Edge.Weight - FoodBonus * Edge.Food;
				Edges.Add(FBellmanFordEdge(Node, Edge.ToNode, Weight));
			}
		}
	}

	return Edges;
}

bool ABellmanFordGhost::HasNegativeCycle(const TArray<FBellmanFordEdge>& Edges, const TArray<int32>& Distances) const
{
	for (const FBellmanFordEdge& Edge : Edges)
	{
		if (Distances[Edge.From] != MAX_int32 && Distances[Edge.From] + Edge.Weight < Distances[Edge.To])
		{
			return true;
		}
	}
	return false;
}
