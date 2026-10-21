// Fill out your copyright notice in the Description page of Project Settings.


#include "Pacman.h"
#include "../Game/PacmanGameMode.h"
#include "../Ghost/Ghost.h"
#include "../Map/PacmanMap.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EngineUtils.h"
#include "Engine/LocalPlayer.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

APacman::APacman()
{
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetSphereRadius(40.f);
	Collision->SetCollisionProfileName(TEXT("Pawn"));
	Collision->SetGenerateOverlapEvents(true);
	RootComponent = Collision;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(Collision);
	Body->SetStaticMesh(SphereMesh.Object);
	Body->SetRelativeScale3D(FVector(0.8f));
	Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APacman::BeginPlay()
{
	Super::BeginPlay();

	Map = Cast<APacmanMap>(UGameplayStatics::GetActorOfClass(this, APacmanMap::StaticClass()));
	if (Map == nullptr)
	{
		return;
	}

	CurrentTile = Map->GetLayout().PacmanStart;
	NextTile = CurrentTile;

	FVector Location = Map->TileToWorld(CurrentTile);
	Location.Z += Map->GetTileSize() / 2.f;
	SetActorLocation(Location);
}

void APacman::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController == nullptr)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (Subsystem == nullptr)
	{
		return;
	}

	if (MappingContext != nullptr)
	{
		Subsystem->AddMappingContext(MappingContext, 0);
	}

	CreateRestartInput();
	Subsystem->AddMappingContext(RestartContext, 1);
}

void APacman::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInput == nullptr)
	{
		return;
	}

	if (MoveAction != nullptr)
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APacman::Move);
	}

	CreateRestartInput();
	EnhancedInput->BindAction(RestartAction, ETriggerEvent::Started, this, &APacman::Restart);
}

void APacman::Move(const FInputActionValue& Value)
{
	FVector2D Input = Value.Get<FVector2D>();

	if (FMath::Abs(Input.X) > FMath::Abs(Input.Y))
	{
		WantedDirection = FIntPoint(Input.X > 0 ? 1 : -1, 0);
	}
	else if (Input.Y != 0)
	{
		WantedDirection = FIntPoint(0, Input.Y > 0 ? -1 : 1);
	}
}

bool APacman::CanMove(FIntPoint Direction) const
{
	if (Direction == FIntPoint::ZeroValue)
	{
		return false;
	}

	FIntPoint Tile = Map->GetGraph().GetNextTile(CurrentTile, Direction);
	return Map->GetLayout().IsWalkable(Tile);
}

void APacman::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Map == nullptr)
	{
		return;
	}

	UpdateMovement(DeltaTime);
	CheckGhosts();
}

void APacman::UpdateMovement(float DeltaTime)
{
	if (WantedDirection == FMazeGraph::Opposite(CurrentDirection) && NextTile != CurrentTile)
	{
		Swap(CurrentTile, NextTile);
		CurrentDirection = WantedDirection;
	}

	float DistanceLeft = Speed * DeltaTime;
	while (DistanceLeft > 0.f)
	{
		if (NextTile == CurrentTile)
		{
			if (CanMove(WantedDirection))
			{
				CurrentDirection = WantedDirection;
			}
			if (!CanMove(CurrentDirection))
			{
				return;
			}
			NextTile = Map->GetGraph().GetNextTile(CurrentTile, CurrentDirection);
		}

		FVector Location = GetActorLocation();
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
			SetActorLocation(Location + MoveDirection * DistanceLeft);
			return;
		}

		SetActorLocation(TargetLocation);
		DistanceLeft -= DistanceToTarget;
		CurrentTile = NextTile;
	}
}

void APacman::CheckGhosts()
{
	APacmanGameMode* GameMode = GetWorld()->GetAuthGameMode<APacmanGameMode>();
	if (GameMode == nullptr)
	{
		return;
	}

	float CatchDistance = Map->GetTileSize() / 2.f;
	for (TActorIterator<AGhost> It(GetWorld()); It; ++It)
	{
		if (FVector::Dist2D(It->GetActorLocation(), GetActorLocation()) < CatchDistance)
		{
			GameMode->OnGhostTouched(*It);
		}
	}
}

void APacman::CreateRestartInput()
{
	if (RestartAction != nullptr)
	{
		return;
	}

	RestartAction = NewObject<UInputAction>(this);
	RestartAction->bTriggerWhenPaused = true;

	RestartContext = NewObject<UInputMappingContext>(this);
	RestartContext->MapKey(RestartAction, EKeys::R);
}

void APacman::Restart()
{
	APacmanGameMode* GameMode = GetWorld()->GetAuthGameMode<APacmanGameMode>();
	if (GameMode != nullptr)
	{
		GameMode->RestartGame();
	}
}
