#include "TerrainCameraPawn.h"
#include "VoxelTerrain.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerInput.h"
#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
#include "UnrealClient.h"
#if WITH_EDITOR
#include "ShaderCompiler.h"
#endif

namespace
{
	const FName SpeedUpAction(TEXT("TerrainCamera_SpeedUp"));
	const FName SpeedDownAction(TEXT("TerrainCamera_SpeedDown"));
	const FName BoostAction(TEXT("TerrainCamera_Boost"));

	void AddTerrainCameraBindings()
	{
		static bool bBindingsAdded = false;
		if (!bBindingsAdded)
		{
			bBindingsAdded = true;
			UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping(SpeedUpAction, EKeys::MouseScrollUp));
			UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping(SpeedDownAction, EKeys::MouseScrollDown));
			UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping(BoostAction, EKeys::LeftShift));
			UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping(BoostAction, EKeys::Gamepad_LeftShoulder));
		}
	}
}

ATerrainCameraPawn::ATerrainCameraPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bAddDefaultMovementBindings = true;
	GetCollisionComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMeshComponent()->SetHiddenInGame(true);
}

void ATerrainCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	for (TActorIterator<AVoxelTerrain> It(GetWorld()); It; ++It)
	{
		Terrain = *It;
		break;
	}
	ApplySpeed();
	FParse::Value(FCommandLine::Get(), TEXT("AutoScreenshot="), AutoScreenshotAfter);
}

void ATerrainCameraPawn::ApplyCommandLinePose()
{
	// -CameraPose=X,Y,Height,Pitch,Yaw : terrain-local metres, height above ground in metres, degrees.
	FString Pose;
	const AVoxelTerrain* T = Terrain.Get();
	if (!T || !FParse::Value(FCommandLine::Get(), TEXT("CameraPose="), Pose, false))
	{
		return;
	}
	TArray<FString> Parts;
	Pose.ParseIntoArray(Parts, TEXT(","));
	if (Parts.Num() != 5)
	{
		return;
	}
	FVector Location = T->GetActorLocation() + FVector(FCString::Atod(*Parts[0]), FCString::Atod(*Parts[1]), 0.0) * 100.0;
	Location.Z = T->GetGroundHeightAt(Location) + FCString::Atod(*Parts[2]) * 100.0;
	const FRotator Rotation(FCString::Atod(*Parts[3]), FCString::Atod(*Parts[4]), 0.0);
	SetActorLocation(Location);
	if (AController* PC = GetController())
	{
		PC->SetControlRotation(Rotation);
	}
}

void ATerrainCameraPawn::TickAutoScreenshot(float DeltaSeconds)
{
	if (AutoScreenshotAfter < 0.0f)
	{
		return;
	}
	AutoScreenshotTimer += DeltaSeconds;
	const AVoxelTerrain* T = Terrain.Get();
	bool bShadersReady = true;
#if WITH_EDITOR
	// Uncooked runs compile material shaders on demand and draw the default checker material until done.
	bShadersReady = !GShaderCompilingManager || GShaderCompilingManager->GetNumRemainingJobs() == 0;
#endif
	if (QuitAt < 0.0f && AutoScreenshotTimer >= AutoScreenshotAfter && bShadersReady && (!T || T->GetNumBuilding() == 0))
	{
		FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir() / TEXT("Screenshots/AutoShot.png"), false, false);
		QuitAt = AutoScreenshotTimer + 2.0f;
	}
	else if (QuitAt >= 0.0f && AutoScreenshotTimer >= QuitAt)
	{
		FPlatformMisc::RequestExit(false);
	}
}

void ATerrainCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	AddTerrainCameraBindings();
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(SpeedUpAction, IE_Pressed, this, &ATerrainCameraPawn::SpeedUp);
	PlayerInputComponent->BindAction(SpeedDownAction, IE_Pressed, this, &ATerrainCameraPawn::SpeedDown);
	PlayerInputComponent->BindAction(BoostAction, IE_Pressed, this, &ATerrainCameraPawn::BoostPressed);
	PlayerInputComponent->BindAction(BoostAction, IE_Released, this, &ATerrainCameraPawn::BoostReleased);
}

void ATerrainCameraPawn::SpeedUp()
{
	SpeedScale = FMath::Min(SpeedScale * 1.25f, 50.0f);
	ApplySpeed();
}

void ATerrainCameraPawn::SpeedDown()
{
	SpeedScale = FMath::Max(SpeedScale / 1.25f, 0.05f);
	ApplySpeed();
}

void ATerrainCameraPawn::BoostPressed()
{
	bBoost = true;
	ApplySpeed();
}

void ATerrainCameraPawn::BoostReleased()
{
	bBoost = false;
	ApplySpeed();
}

void ATerrainCameraPawn::ApplySpeed()
{
	if (UFloatingPawnMovement* Movement = Cast<UFloatingPawnMovement>(GetMovementComponent()))
	{
		Movement->MaxSpeed = BaseSpeed * SpeedScale * (bBoost ? BoostMultiplier : 1.0f);
		Movement->Acceleration = Movement->MaxSpeed * 4.0f;
		Movement->Deceleration = Movement->MaxSpeed * 8.0f;
	}
}

void ATerrainCameraPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// The pawn is possessed after BeginPlay, so the start pose is applied on the first tick.
	if (!bPoseApplied && GetController())
	{
		bPoseApplied = true;
		ApplyCommandLinePose();
	}
	TickAutoScreenshot(DeltaSeconds);

	if (bShowHelp && GEngine)
	{
		const UFloatingPawnMovement* Movement = Cast<UFloatingPawnMovement>(GetMovementComponent());
		const float Speed = Movement ? Movement->MaxSpeed / 100.0f : 0.0f;
		FString Status = FString::Printf(TEXT("Speed %.1f m/s"), Speed);
		if (const AVoxelTerrain* T = Terrain.Get())
		{
			const float Altitude = (GetActorLocation().Z - T->GetGroundHeightAt(GetActorLocation())) / 100.0f;
			Status += FString::Printf(TEXT(" | %.1f m above ground | chunks %d (building %d)"), Altitude, T->GetNumChunks(), T->GetNumBuilding());
		}
		GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::White, Status);
		GEngine->AddOnScreenDebugMessage(2, 0.0f, FColor(200, 200, 200),
			TEXT("WASD move | E/Space up, Q/C down | mouse look | wheel speed | Shift boost"));
	}
}
