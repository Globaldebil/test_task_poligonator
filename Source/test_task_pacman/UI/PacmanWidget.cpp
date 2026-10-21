// Fill out your copyright notice in the Description page of Project Settings.


#include "PacmanWidget.h"
#include "../Game/PacmanGameMode.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Styling/CoreStyle.h"

TSharedRef<SWidget> UPacmanWidget::RebuildWidget()
{
	if (WidgetTree != nullptr && WidgetTree->RootWidget == nullptr)
	{
		UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("Root"));
		WidgetTree->RootWidget = Root;

		ScoreText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ScoreText"));
		ScoreText->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 32));
		ScoreText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		ScoreText->SetText(FText::FromString(TEXT("SCORE: 0")));

		UCanvasPanelSlot* ScoreSlot = Root->AddChildToCanvas(ScoreText);
		ScoreSlot->SetPosition(FVector2D(40.f, 30.f));
		ScoreSlot->SetAutoSize(true);

		TimeText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("TimeText"));
		TimeText->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 32));
		TimeText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		TimeText->SetText(FText::FromString(TEXT("TIME: 00:00")));

		UCanvasPanelSlot* TimeSlot = Root->AddChildToCanvas(TimeText);
		TimeSlot->SetAnchors(FAnchors(1.f, 0.f));
		TimeSlot->SetAlignment(FVector2D(1.f, 0.f));
		TimeSlot->SetPosition(FVector2D(-40.f, 30.f));
		TimeSlot->SetAutoSize(true);

		MessageText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("MessageText"));
		MessageText->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 64));
		MessageText->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
		MessageText->SetJustification(ETextJustify::Center);
		MessageText->SetVisibility(ESlateVisibility::Collapsed);

		UCanvasPanelSlot* MessageSlot = Root->AddChildToCanvas(MessageText);
		MessageSlot->SetAnchors(FAnchors(0.5f, 0.5f));
		MessageSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		MessageSlot->SetPosition(FVector2D(0.f, -60.f));
		MessageSlot->SetAutoSize(true);

		RestartButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("RestartButton"));
		RestartButton->SetVisibility(ESlateVisibility::Collapsed);

		UTextBlock* ButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("RestartButtonText"));
		ButtonText->SetFont(FCoreStyle::GetDefaultFontStyle("Bold", 32));
		ButtonText->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
		ButtonText->SetText(FText::FromString(TEXT("RESTART")));
		RestartButton->AddChild(ButtonText);

		UCanvasPanelSlot* ButtonSlot = Root->AddChildToCanvas(RestartButton);
		ButtonSlot->SetAnchors(FAnchors(0.5f, 0.5f));
		ButtonSlot->SetAlignment(FVector2D(0.5f, 0.5f));
		ButtonSlot->SetPosition(FVector2D(0.f, 90.f));
		ButtonSlot->SetAutoSize(true);
	}

	return Super::RebuildWidget();
}

void UPacmanWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton != nullptr)
	{
		RestartButton->OnClicked.AddUniqueDynamic(this, &UPacmanWidget::OnRestartClicked);
	}
}

void UPacmanWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	APacmanGameMode* GameMode = GetWorld()->GetAuthGameMode<APacmanGameMode>();
	if (GameMode != nullptr)
	{
		SetTime(GameMode->GetElapsedTime());
	}
}

void UPacmanWidget::SetScore(int32 Score)
{
	if (ScoreText != nullptr)
	{
		ScoreText->SetText(FText::FromString(FString::Printf(TEXT("SCORE: %d"), Score)));
	}
}

void UPacmanWidget::SetTime(float Seconds)
{
	if (TimeText == nullptr)
	{
		return;
	}

	int32 TotalSeconds = FMath::FloorToInt(Seconds);
	int32 Minutes = TotalSeconds / 60;
	int32 RemainingSeconds = TotalSeconds % 60;
	TimeText->SetText(FText::FromString(FString::Printf(TEXT("TIME: %02d:%02d"), Minutes, RemainingSeconds)));
}

void UPacmanWidget::ShowEndScreen(const FString& Message)
{
	if (MessageText != nullptr)
	{
		MessageText->SetText(FText::FromString(Message));
		MessageText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (RestartButton != nullptr)
	{
		RestartButton->SetVisibility(ESlateVisibility::Visible);
	}
}

void UPacmanWidget::OnRestartClicked()
{
	APacmanGameMode* GameMode = GetWorld()->GetAuthGameMode<APacmanGameMode>();
	if (GameMode != nullptr)
	{
		GameMode->RestartGame();
	}
}
