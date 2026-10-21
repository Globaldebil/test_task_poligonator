// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostAIController.h"
#include "Ghost.h"
#include "../Map/PacmanMap.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AGhostAIController::AGhostAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGhostAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	Ghost = Cast<AGhost>(InPawn);
	bInitialized = false;
}

void AGhostAIController::OnUnPossess()
{
	Super::OnUnPossess();

	Ghost = nullptr;
}

void AGhostAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Ghost == nullptr)
	{
		return;
	}

	if (!bInitialized)
	{
		bInitialized = InitPosition();
		if (!bInitialized)
		{
			return;
		}
	}

	MoveGhost(DeltaTime);

	if (Ghost->ShouldDrawPath())
	{
		DrawPath();
	}
}

bool AGhostAIController::InitPosition()
{
	Map = Cast<APacmanMap>(UGameplayStatics::GetActorOfClass(this, APacmanMap::StaticClass()));
	if (Map == nullptr)
	{
		return false;
	}

	CurrentTile = Map->WorldToTile(Ghost->GetActorLocation());
	NextTile = CurrentTile;
	CurrentDirection = FIntPoint::ZeroValue;
	PathTiles.Empty();

	FVector Location = Map->TileToWorld(CurrentTile);
	Location.Z = Ghost->GetActorLocation().Z;
	Ghost->SetActorLocation(Location);

	return true;
}

void AGhostAIController::MoveGhost(float DeltaTime)
{
	float DistanceLeft = Ghost->GetCharacterMovement()->MaxWalkSpeed * DeltaTime;

	while (DistanceLeft > 0.f)
	{
		if (NextTile == CurrentTile)
		{
			CurrentDirection = ChooseDirection();
			NextTile = Map->GetGraph().GetNextTile(CurrentTile, CurrentDirection);
			if (NextTile == CurrentTile)
			{
				return;
			}
		}

		FVector Location = Ghost->GetActorLocation();
		FVector TargetLocation = Map->TileToWorld(NextTile);
		TargetLocation.Z = Location.Z;

		bool bThroughTunnel = FMath::Abs(NextTile.X - CurrentTile.X) > 1;
		if (bThroughTunnel)
		{
			Location = TargetLocation;
		}

		float DistanceToTarget = FVector::Dist(Location, TargetLocation);
		if (DistanceToTarget > DistanceLeft)
		{
			FVector MoveDirection = (TargetLocation - Location).GetSafeNormal();
			Ghost->SetActorLocation(Location + MoveDirection * DistanceLeft);
			return;
		}

		Ghost->SetActorLocation(TargetLocation);
		DistanceLeft -= DistanceToTarget;
		CurrentTile = NextTile;
	}
}

FIntPoint AGhostAIController::ChooseDirection()
{
	const FMazeGraph& Graph = Map->GetGraph();

	bool bInCorridor = Graph.FindNode(CurrentTile) == INDEX_NONE;
	if (bInCorridor && CurrentDirection != FIntPoint::ZeroValue && Graph.IsWalkable(Graph.GetNextTile(CurrentTile, CurrentDirection)))
	{
		return CurrentDirection;
	}

	if (Ghost->FollowsWholePath() && PlannedState == Ghost->GetState())
	{
		int32 Index = PathTiles.Find(CurrentTile);
		if (Index != INDEX_NONE && Index + 1 < PathTiles.Num())
		{
			return GetDirectionBetween(PathTiles[Index], PathTiles[Index + 1]);
		}
	}

	FMazeGraph SearchGraph = Graph;
	int32 StartNode = SearchGraph.GetOrCreateNode(CurrentTile);
	if (StartNode == INDEX_NONE)
	{
		return FIntPoint::ZeroValue;
	}

	if (SearchGraph.GetNode(StartNode).Edges.Num() > 1)
	{
		SearchGraph.RemoveEdge(StartNode, FMazeGraph::Opposite(CurrentDirection));
	}

	PathTiles.Empty();
	PlannedState = Ghost->GetState();

	if (Ghost->GetState() == EGhostState::Fearing)
	{
		return GetFleeDirection(SearchGraph, StartNode);
	}

	FIntPoint TargetTile;
	if (!FindTargetTile(TargetTile))
	{
		return GetRandomDirection(SearchGraph, StartNode);
	}

	if (TargetTile == CurrentTile)
	{
		return FIntPoint::ZeroValue;
	}

	int32 TargetNode = SearchGraph.GetOrCreateNode(TargetTile);
	if (TargetNode == INDEX_NONE)
	{
		return GetRandomDirection(SearchGraph, StartNode);
	}

	SearchGraph.CountFood(Map->GetPickupTiles());

	TArray<int32> Path = Ghost->FindPath(SearchGraph, StartNode, TargetNode);
	if (Path.Num() < 2)
	{
		return GetRandomDirection(SearchGraph, StartNode);
	}

	for (int32 Node : Path)
	{
		PathTiles.Add(SearchGraph.GetNode(Node).Tile);
	}

	return GetDirectionToNode(SearchGraph, StartNode, Path[1]);
}

FIntPoint AGhostAIController::GetRandomDirection(const FMazeGraph& Graph, int32 Node) const
{
	const TArray<FGraphEdge>& Edges = Graph.GetNode(Node).Edges;
	if (Edges.Num() == 0)
	{
		return FIntPoint::ZeroValue;
	}

	int32 Index = FMath::RandRange(0, Edges.Num() - 1);
	return Edges[Index].Direction;
}

FIntPoint AGhostAIController::GetFleeDirection(const FMazeGraph& Graph, int32 Node) const
{
	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	if (Player == nullptr)
	{
		return GetRandomDirection(Graph, Node);
	}

	FIntPoint PlayerTile = Map->WorldToTile(Player->GetActorLocation());
	FIntPoint BestDirection = FIntPoint::ZeroValue;
	int32 BestDistance = -1;

	for (const FGraphEdge& Edge : Graph.GetNode(Node).Edges)
	{
		FIntPoint Tile = Graph.GetNode(Edge.ToNode).Tile;
		int32 Distance = FMath::Abs(Tile.X - PlayerTile.X) + FMath::Abs(Tile.Y - PlayerTile.Y);
		if (Distance > BestDistance)
		{
			BestDistance = Distance;
			BestDirection = Edge.Direction;
		}
	}

	return BestDirection;
}

FIntPoint AGhostAIController::GetDirectionToNode(const FMazeGraph& Graph, int32 From, int32 To) const
{
	for (const FGraphEdge& Edge : Graph.GetNode(From).Edges)
	{
		if (Edge.ToNode == To)
		{
			return Edge.Direction;
		}
	}
	return FIntPoint::ZeroValue;
}

FIntPoint AGhostAIController::GetDirectionBetween(FIntPoint From, FIntPoint To) const
{
	int32 DeltaX = To.X - From.X;
	int32 DeltaY = To.Y - From.Y;

	if (FMath::Abs(DeltaX) > Map->GetLayout().Width / 2)
	{
		DeltaX = -DeltaX;
	}

	return FIntPoint(FMath::Sign(DeltaX), FMath::Sign(DeltaY));
}

bool AGhostAIController::FindTargetTile(FIntPoint& OutTile) const
{
	if (Ghost->GetState() == EGhostState::Eaten)
	{
		OutTile = Map->GetLayout().HouseCenter;
		return true;
	}

	if (Ghost->GetState() == EGhostState::Fearing)
	{
		return false;
	}

	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	if (Player == nullptr)
	{
		return false;
	}

	OutTile = Map->WorldToTile(Player->GetActorLocation());
	return Map->GetGraph().IsWalkable(OutTile);
}

void AGhostAIController::DrawPath() const
{
	FColor Color = Ghost->GetBodyColor().ToFColor(true);
	float Height = Ghost->GetActorLocation().Z;
	int32 HalfWidth = Map->GetLayout().Width / 2;

	for (int32 i = 1; i < PathTiles.Num(); i++)
	{
		bool bThroughTunnel = FMath::Abs(PathTiles[i].X - PathTiles[i - 1].X) > HalfWidth;
		if (bThroughTunnel)
		{
			continue;
		}

		FVector Start = Map->TileToWorld(PathTiles[i - 1]);
		FVector End = Map->TileToWorld(PathTiles[i]);
		Start.Z = Height;
		End.Z = Height;
		DrawDebugLine(GetWorld(), Start, End, Color, false, -1.f, 0, 4.f);
	}
}
