#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "TerrainCameraPawn.generated.h"

class AVoxelTerrain;

/**
 * Free-flying camera with the standard Unreal controls:
 * WASD / arrows move, E / Space up, Q / C down, mouse look, gamepad sticks.
 * Additionally: mouse wheel changes speed, hold Left Shift to boost.
 */
UCLASS()
class UNREALTEST_API ATerrainCameraPawn : public ADefaultPawn
{
	GENERATED_BODY()

public:
	ATerrainCameraPawn(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/** Base flying speed in cm/s. */
	UPROPERTY(EditAnywhere, Category = "Camera")
	float BaseSpeed = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float BoostMultiplier = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	bool bShowHelp = true;

protected:
	virtual void BeginPlay() override;

private:
	void SpeedUp();
	void SpeedDown();
	void BoostPressed();
	void BoostReleased();
	void ApplySpeed();
	void ApplyCommandLinePose();
	void TickAutoScreenshot(float DeltaSeconds);

	/** -AutoScreenshot=<seconds>: once streaming is idle after this delay, save Saved/Screenshots/AutoShot.png and quit. */
	float AutoScreenshotAfter = -1.0f;
	float AutoScreenshotTimer = 0.0f;
	float QuitAt = -1.0f;

	bool bPoseApplied = false;
	float SpeedScale = 1.0f;
	bool bBoost = false;
	TWeakObjectPtr<AVoxelTerrain> Terrain;
};
