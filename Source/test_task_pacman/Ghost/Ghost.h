// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Ghost.generated.h"

class FMazeGraph;
class UMaterialInterface;

UENUM(BlueprintType)
enum class EGhostState : uint8
{
	Hunting,
	Fearing,
	Eaten
};



UCLASS(Abstract)
class TEST_TASK_PACMAN_API AGhost : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGhost();
	EGhostState GetState() const { return State; }
	const FLinearColor& GetBodyColor() const { return BodyColor; }
	bool ShouldDrawPath() const { return bDrawPath; }
	void StartFearing();
	void GotEaten();

	virtual TArray<int32> FindPath(const FMazeGraph& Graph, int32 From, int32 To) const PURE_VIRTUAL(AGhost::FindPath, return {};);
	virtual bool FollowsWholePath() const { return false; }

private:
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> BodyMaterial;
	
	void ApplyBodyColor(const FLinearColor& Color);
	FTimerHandle FearTimer;
	FTimerHandle RespawnTimer;

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInterface>> NormalMaterials;

	void SetFearLook(bool bFear);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Ghost|Speed")
	float HuntingSpeed = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "Ghost|Speed")
	float FearingSpeed = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Ghost|Speed")
	float EatenSpeed = 600.f;
	
	UPROPERTY(VisibleAnywhere, Category = "Ghost")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Ghost|State")
	float FearTime = 7.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ghost|State")
	float RespawnDelay = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Ghost|Visual")
	FLinearColor BodyColor;
 
	UPROPERTY(EditDefaultsOnly, Category = "Ghost|Visual")
	FLinearColor FearingColor = FLinearColor::Blue;

	UPROPERTY(EditAnywhere, Category = "Ghost|Visual")
	TObjectPtr<UMaterialInterface> FearMaterial;
	
	UPROPERTY(VisibleAnywhere, Category = "Ghost")
	EGhostState State = EGhostState::Hunting;

	UPROPERTY(EditAnywhere, Category = "Ghost|Debug")
	bool bDrawPath = false;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void StartHunting();

	static TArray<int32> BuildPath(const TMap<int32, int32>& Parents, int32 From, int32 To);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
