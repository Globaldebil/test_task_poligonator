// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PacmanWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class TEST_TASK_PACMAN_API UPacmanWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetScore(int32 Score);
	void SetTime(float Seconds);
	void ShowEndScreen(const FString& Message);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ScoreText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TimeText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MessageText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> RestartButton;

private:
	UFUNCTION()
	void OnRestartClicked();
};
