#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UnrealTestGameMode.generated.h"

/** Uses the terrain camera pawn and makes sure the level contains a voxel terrain. */
UCLASS()
class UNREALTEST_API AUnrealTestGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUnrealTestGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
};
