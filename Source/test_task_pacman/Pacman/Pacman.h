// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Pacman.generated.h"

class APacmanMap;
class USphereComponent;
class UStaticMeshComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class TEST_TASK_PACMAN_API APacman : public APawn
{
	GENERATED_BODY()

public:
	APacman();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void NotifyControllerChanged() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Pacman")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, Category = "Pacman")
	TObjectPtr<UStaticMeshComponent> Body;

	UPROPERTY(EditAnywhere, Category = "Pacman")
	float Speed = 400.f;

	UPROPERTY(EditAnywhere, Category = "Pacman|Input")
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditAnywhere, Category = "Pacman|Input")
	TObjectPtr<UInputAction> MoveAction;

private:
	void Move(const FInputActionValue& Value);
	bool CanMove(FIntPoint Direction) const;
	void UpdateMovement(float DeltaTime);
	void CheckGhosts();
	void CreateRestartInput();
	void Restart();

	UPROPERTY()
	TObjectPtr<APacmanMap> Map;

	UPROPERTY()
	TObjectPtr<UInputAction> RestartAction;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> RestartContext;

	FIntPoint CurrentTile;
	FIntPoint NextTile;
	FIntPoint CurrentDirection = FIntPoint::ZeroValue;
	FIntPoint WantedDirection = FIntPoint::ZeroValue;
};
