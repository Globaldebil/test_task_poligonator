// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PacmanGameMode.generated.h"

class AGhost;
class APickup;
class UPacmanWidget;

UCLASS()
class TEST_TASK_PACMAN_API APacmanGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APacmanGameMode();

	void OnPickupEaten(APickup* Pickup);
	void FrightenGhosts();
	void SetTotalPickups(int32 Count);
	void OnGhostTouched(AGhost* Ghost);
	void RestartGame();

	int32 GetScore() const { return Score; }
	int32 GetEatenPickups() const { return EatenPickups; }
	bool IsGameOver() const { return bGameOver; }
	float GetElapsedTime() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPacmanWidget> WidgetClass;

private:
	void AddScore(int32 Points);
	void Win();
	void Lose();
	void EndGame(const FString& Message);

	UPROPERTY()
	TObjectPtr<UPacmanWidget> Widget;

	int32 Score = 0;
	int32 EatenPickups = 0;
	int32 TotalPickups = 0;
	bool bGameOver = false;
	float StartTime = 0.f;
};
