// Fill out your copyright notice in the Description page of Project Settings.


#include "PacmanGameMode.h"
#include "../Ghost/Ghost.h"
#include "../Pickup/Pickup.h"
#include "../UI/PacmanWidget.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

APacmanGameMode::APacmanGameMode()
{
	WidgetClass = UPacmanWidget::StaticClass();
}

void APacmanGameMode::BeginPlay()
{
	Super::BeginPlay();

	StartTime = GetWorld()->GetTimeSeconds();

	if (WidgetClass != nullptr)
	{
		Widget = CreateWidget<UPacmanWidget>(GetWorld(), WidgetClass);
		Widget->AddToViewport();
		Widget->SetScore(Score);
	}
}

void APacmanGameMode::OnPickupEaten(APickup* Pickup)
{
	if (bGameOver)
	{
		return;
	}

	AddScore(Pickup->GetPoints());
	EatenPickups++;

	if (EatenPickups == TotalPickups)
	{
		Win();
	}
}

void APacmanGameMode::SetTotalPickups(int32 Count)
{
	TotalPickups = Count;
}

void APacmanGameMode::OnGhostTouched(AGhost* Ghost)
{
	if (bGameOver)
	{
		return;
	}

	if (Ghost->GetState() == EGhostState::Fearing)
	{
		Ghost->GotEaten();
		AddScore(200);
		return;
	}

	if (Ghost->GetState() == EGhostState::Hunting)
	{
		Lose();
	}
}

void APacmanGameMode::RestartGame()
{
	if (!bGameOver)
	{
		return;
	}

	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this)));
}

float APacmanGameMode::GetElapsedTime() const
{
	return GetWorld()->GetTimeSeconds() - StartTime;
}

void APacmanGameMode::FrightenGhosts()
{
	for (TActorIterator<AGhost> It(GetWorld()); It; ++It)
	{
		It->StartFearing();
	}
}

void APacmanGameMode::AddScore(int32 Points)
{
	Score += Points;

	if (Widget != nullptr)
	{
		Widget->SetScore(Score);
	}
}

void APacmanGameMode::Win()
{
	EndGame(TEXT("YOU WIN!\nPRESS R TO RESTART"));
}

void APacmanGameMode::Lose()
{
	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	if (Player != nullptr)
	{
		Player->SetActorHiddenInGame(true);
	}

	EndGame(TEXT("GAME OVER\nPRESS R TO RESTART"));
}

void APacmanGameMode::EndGame(const FString& Message)
{
	bGameOver = true;

	if (Widget != nullptr)
	{
		Widget->ShowEndScreen(Message);
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController != nullptr)
	{
		PlayerController->SetShowMouseCursor(true);
		PlayerController->SetInputMode(FInputModeGameAndUI());
	}

	UGameplayStatics::SetGamePaused(this, true);
}
