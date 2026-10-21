// Fill out your copyright notice in the Description page of Project Settings.


#include "Ghost.h"
#include "GhostAIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

// Sets default values
AGhost::AGhost()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
 
	// Меш только для картинки, столкновения ловит капсула
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetCapsuleComponent());
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 
	// Overlap вместо Block: призрак должен ловить Пакмана, а не толкать
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	GetCharacterMovement()->DefaultLandMovementMode = MOVE_Flying;

	AIControllerClass = AGhostAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AGhost::BeginPlay()
{
	Super::BeginPlay();
	BodyMaterial = BodyMesh->CreateDynamicMaterialInstance(0);

	for (int32 i = 0; i < BodyMesh->GetNumMaterials(); i++)
	{
		NormalMaterials.Add(BodyMesh->GetMaterial(i));
	}

	StartHunting();
}

// Called every frame
void AGhost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGhost::StartHunting()
{
	GetWorldTimerManager().ClearTimer(FearTimer);
	GetWorldTimerManager().ClearTimer(RespawnTimer);
 
	State = EGhostState::Hunting;
	GetCharacterMovement()->MaxWalkSpeed = HuntingSpeed;
	BodyMesh->SetVisibility(true);
	ApplyBodyColor(BodyColor);
	SetFearLook(false);
}

void AGhost::StartFearing()
{
	if (State == EGhostState::Eaten)
	{
		return;
	}
 
	State = EGhostState::Fearing;
	GetCharacterMovement()->MaxWalkSpeed = FearingSpeed;
	ApplyBodyColor(FearingColor);
	SetFearLook(true);
	
	GetWorldTimerManager().SetTimer(FearTimer, this, &AGhost::StartHunting, FearTime, false);
}
 
void AGhost::GotEaten()
{
	if (State == EGhostState::Eaten)
	{
		return;
	}
 
	GetWorldTimerManager().ClearTimer(FearTimer);
 
	State = EGhostState::Eaten;
	GetCharacterMovement()->MaxWalkSpeed = EatenSpeed;
	BodyMesh->SetVisibility(false);
	
	GetWorldTimerManager().SetTimer(RespawnTimer, this, &AGhost::StartHunting, RespawnDelay, false);
}
 
void AGhost::ApplyBodyColor(const FLinearColor& Color)
{
	static const FName ColorParam(TEXT("Color"));
 
	if (BodyMaterial)
	{
		BodyMaterial->SetVectorParameterValue(ColorParam, Color);
	}
}

TArray<int32> AGhost::BuildPath(const TMap<int32, int32>& Parents, int32 From, int32 To)
{
	TArray<int32> Path;
	if (!Parents.Contains(To))
	{
		return Path;
	}

	int32 Current = To;
	while (Current != From)
	{
		Path.Insert(Current, 0);
		Current = Parents[Current];
	}
	Path.Insert(From, 0);

	return Path;
}

void AGhost::SetFearLook(bool bFear)
{
	if (FearMaterial == nullptr)
	{
		return;
	}

	for (int32 i = 0; i < NormalMaterials.Num(); i++)
	{
		BodyMesh->SetMaterial(i, bFear ? FearMaterial.Get() : NormalMaterials[i].Get());
	}
}
