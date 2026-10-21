// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Ghost.h"
#include "GhostAIController.generated.h"

class AGhost;
class APacmanMap;
class FMazeGraph;

UCLASS()
class TEST_TASK_PACMAN_API AGhostAIController : public AAIController
{
	GENERATED_BODY()

public:
	AGhostAIController();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	bool InitPosition();
	void MoveGhost(float DeltaTime);
	FIntPoint ChooseDirection();
	FIntPoint GetRandomDirection(const FMazeGraph& Graph, int32 Node) const;
	FIntPoint GetFleeDirection(const FMazeGraph& Graph, int32 Node) const;
	FIntPoint GetDirectionToNode(const FMazeGraph& Graph, int32 From, int32 To) const;
	FIntPoint GetDirectionBetween(FIntPoint From, FIntPoint To) const;
	bool FindTargetTile(FIntPoint& OutTile) const;
	void DrawPath() const;

	UPROPERTY()
	TObjectPtr<AGhost> Ghost;

	UPROPERTY()
	TObjectPtr<APacmanMap> Map;

	bool bInitialized = false;
	FIntPoint CurrentTile;
	FIntPoint NextTile;
	FIntPoint CurrentDirection = FIntPoint::ZeroValue;
	TArray<FIntPoint> PathTiles;
	EGhostState PlannedState = EGhostState::Hunting;
};
