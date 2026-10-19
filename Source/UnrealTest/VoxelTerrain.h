#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "GameFramework/Actor.h"
#include <atomic>
#include "VoxelTerrain.generated.h"

class FVoxelHeightField;
class UMaterialInterface;
class UProceduralMeshComponent;
struct FVoxelChunkMeshData;
struct FVoxelMeshBuffers;

/** Quadtree node id: Level 0 = root (coarsest), X/Y = node index at that level. */
struct FVoxelChunkKey
{
	int32 Level = 0;
	int32 X = 0;
	int32 Y = 0;

	bool operator==(const FVoxelChunkKey& Other) const { return Level == Other.Level && X == Other.X && Y == Other.Y; }
	friend uint32 GetTypeHash(const FVoxelChunkKey& Key) { return HashCombine(HashCombine(::GetTypeHash(Key.Level), ::GetTypeHash(Key.X)), ::GetTypeHash(Key.Y)); }
};

/**
 * Voxel terrain built from a heightmap. The terrain is a grid of 1 dm voxel columns, far too many
 * to mesh at once (20000 x 20000 for 2 km), so it is streamed as a quadtree of chunks around the
 * camera: every chunk has ColumnsPerChunk^2 columns, and each coarser level doubles the voxel size.
 * Chunks are meshed on worker threads and swapped in once their replacement area is complete.
 *
 * The actor location is the north-west corner of the terrain at the lowest elevation.
 */
UCLASS()
class UNREALTEST_API AVoxelTerrain : public AActor
{
	GENERATED_BODY()

public:
	AVoxelTerrain();

	virtual void Tick(float DeltaSeconds) override;

	/** Ground height (world Z, cm) at a world location; returns the actor Z when the terrain is not loaded. */
	UFUNCTION(BlueprintCallable, Category = "Voxel Terrain")
	float GetGroundHeightAt(const FVector& WorldLocation) const;

	int32 GetNumChunks() const { return Chunks.Num(); }
	int32 GetNumBuilding() const { return Building.Num(); }

	/** Heightmap files, relative to the project Content directory. */
	UPROPERTY(EditAnywhere, Category = "Voxel Terrain|Source")
	FString HeightmapFile = TEXT("Terrain/heightmap.r16");

	UPROPERTY(EditAnywhere, Category = "Voxel Terrain|Source")
	FString HeightmapMetaFile = TEXT("Terrain/heightmap.json");

	UPROPERTY(EditAnywhere, Category = "Voxel Terrain|Rendering")
	TSoftObjectPtr<UMaterialInterface> TerrainMaterial;

	UPROPERTY(EditAnywhere, Category = "Voxel Terrain|Rendering")
	TSoftObjectPtr<UMaterialInterface> WaterMaterial;

	/** Edge length of the finest voxel, in metres. */
	UPROPERTY(EditAnywhere, Category = "Voxel Terrain|Voxels", meta = (ClampMin = "0.01"))
	double BaseVoxelSize = 0.1;

	UPROPERTY(EditAnywhere, Category = "Voxel Terrain|Voxels", meta = (ClampMin = "8", ClampMax = "256"))
	int32 ColumnsPerChunk = 128;

	/** Even out the voxel steps: render a curved, continuous surface through the voxel corners instead of blocky columns. */
	UPROPERTY(EditAnywhere, Category = "Voxel Terrain|Voxels")
	bool bSmoothSurface = true;

	/** Number of quadtree levels; the root level uses voxels of BaseVoxelSize * 2^(LodLevels-1). */
	UPROPERTY(EditAnywhere, Category = "Voxel Terrain|Voxels", meta = (ClampMin = "1", ClampMax = "10"))
	int32 LodLevels = 6;

	/** A node is split when the camera is closer than this many node sizes. */
	UPROPERTY(EditAnywhere, Category = "Voxel Terrain|Streaming", meta = (ClampMin = "0.5"))
	float LodDistanceFactor = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Voxel Terrain|Streaming", meta = (ClampMin = "1"))
	int32 MaxConcurrentBuilds = 6;

	UPROPERTY(EditAnywhere, Category = "Voxel Terrain|Streaming", meta = (ClampMin = "1"))
	int32 MaxMeshUploadsPerTick = 8;

	UPROPERTY(EditAnywhere, Category = "Voxel Terrain|Streaming", meta = (ClampMin = "0.0"))
	float UpdateInterval = 0.1f;

	/** Amplitude (m) of procedural micro relief added on top of the ~4 m DEM. */
	UPROPERTY(EditAnywhere, Category = "Voxel Terrain|Shape", meta = (ClampMin = "0.0"))
	float DetailNoiseAmplitude = 0.25f;

	/** River centreline in terrain-local metres (X = east, Y = south from the actor origin). */
	UPROPERTY(EditAnywhere, Category = "Voxel Terrain|River")
	TArray<FVector2D> RiverControlPoints;

	UPROPERTY(EditAnywhere, Category = "Voxel Terrain|River", meta = (ClampMin = "0.5"))
	float RiverHalfWidth = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Voxel Terrain|River", meta = (ClampMin = "0.1"))
	float RiverDepth = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Voxel Terrain|River", meta = (ClampMin = "0.0"))
	float RiverBankWidth = 3.0f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	struct FChunk
	{
		UProceduralMeshComponent* Ground = nullptr;
		UProceduralMeshComponent* Water = nullptr;
	};

	struct FBuildResult
	{
		FVoxelChunkKey Key;
		TSharedPtr<FVoxelChunkMeshData, ESPMode::ThreadSafe> Data;
	};

	double VoxelSizeAt(int32 Level) const;
	double ChunkSizeAt(int32 Level) const { return VoxelSizeAt(Level) * ColumnsPerChunk; }
	bool GetCameraLocal(FVector& OutLocalMeters) const;

	void UpdateDesired(const FVector& CameraLocal);
	void CollectDesired(const FVector& CameraLocal, double CameraHeightAboveGround, int32 Level, int32 X, int32 Y);
	void StartBuilds();
	void ReceiveBuilds();
	void RemoveReplacedChunks();
	UProceduralMeshComponent* CreateMeshComponent(const FVoxelMeshBuffers& Buffers, UMaterialInterface* Material, const FVector& RelativeLocation, bool bCastShadow);
	void DestroyChunk(FChunk& Chunk);

	static bool Overlaps(const FVoxelChunkKey& A, const FVoxelChunkKey& B);

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> LoadedTerrainMaterial;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> LoadedWaterMaterial;

	TSharedPtr<const FVoxelHeightField, ESPMode::ThreadSafe> Field;
	TSharedPtr<TQueue<FBuildResult, EQueueMode::Mpsc>, ESPMode::ThreadSafe> Completed;
	TSharedPtr<std::atomic<bool>, ESPMode::ThreadSafe> Cancelled;

	TMap<FVoxelChunkKey, FChunk> Chunks;
	TSet<FVoxelChunkKey> Building;
	TSet<FVoxelChunkKey> Desired;
	TArray<FVoxelChunkKey> Missing;
	float TimeSinceUpdate = 0.0f;
};
