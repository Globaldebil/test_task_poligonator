// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UMaterialInterface;
class APacmanGameMode;

UCLASS(Abstract)
class TEST_TASK_PACMAN_API APickup : public AActor
{
	GENERATED_BODY()

public:
	APickup();

	int32 GetPoints() const { return Points; }
	void SetMaterial(UMaterialInterface* Material);

	virtual void OnEaten(APacmanGameMode* GameMode);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, Category = "Pickup")
	int32 Points = 10;

private:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
