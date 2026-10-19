#include "UnrealTestGameMode.h"
#include "TerrainCameraPawn.h"
#include "VoxelTerrain.h"
#include "Engine/World.h"
#include "EngineUtils.h"

AUnrealTestGameMode::AUnrealTestGameMode()
{
	DefaultPawnClass = ATerrainCameraPawn::StaticClass();
}

void AUnrealTestGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// Fallback for levels without a placed terrain: centre the 2 km terrain on the world origin.
	UWorld* World = GetWorld();
	if (World && !TActorIterator<AVoxelTerrain>(World))
	{
		World->SpawnActor<AVoxelTerrain>(FVector(-100000.0, -100000.0, 0.0), FRotator::ZeroRotator);
	}
}
