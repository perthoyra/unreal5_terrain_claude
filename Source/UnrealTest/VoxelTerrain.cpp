#include "VoxelTerrain.h"
#include "UnrealTest.h"
#include "VoxelChunkMesher.h"
#include "VoxelHeightField.h"
#include "Async/Async.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "MaterialDomain.h"
#include "Materials/Material.h"
#include "Misc/Paths.h"
#include "ProceduralMeshComponent.h"

AVoxelTerrain::AVoxelTerrain()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	TerrainMaterial = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/Materials/M_VoxelGround.M_VoxelGround")));
	WaterMaterial = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/Materials/M_Water.M_Water")));

	// A ~1 km meandering river through the low, flat south-east corner (away from the mountain).
	RiverControlPoints = {
		FVector2D(2000.0, 1250.0), FVector2D(1880.0, 1330.0), FVector2D(1760.0, 1300.0),
		FVector2D(1650.0, 1420.0), FVector2D(1600.0, 1560.0), FVector2D(1480.0, 1650.0),
		FVector2D(1450.0, 1800.0), FVector2D(1380.0, 1900.0), FVector2D(1350.0, 2000.0)
	};
}

void AVoxelTerrain::BeginPlay()
{
	Super::BeginPlay();

	const FString ContentDir = FPaths::ProjectContentDir();
	TSharedPtr<FVoxelHeightField, ESPMode::ThreadSafe> NewField = MakeShared<FVoxelHeightField, ESPMode::ThreadSafe>();
	FString Error;
	if (!NewField->Load(ContentDir / HeightmapFile, ContentDir / HeightmapMetaFile, Error))
	{
		UE_LOG(LogUnrealTest, Error, TEXT("VoxelTerrain: %s"), *Error);
		return;
	}
	NewField->SetDetailNoise(DetailNoiseAmplitude);

	FVoxelRiverSettings River;
	River.ControlPoints = RiverControlPoints;
	River.HalfWidth = RiverHalfWidth;
	River.Depth = RiverDepth;
	River.BankWidth = RiverBankWidth;
	NewField->BuildRiver(River);	Field = NewField;

	LoadedTerrainMaterial = TerrainMaterial.LoadSynchronous();
	LoadedWaterMaterial = WaterMaterial.LoadSynchronous();
	if (!LoadedTerrainMaterial)
	{
		UE_LOG(LogUnrealTest, Warning, TEXT("VoxelTerrain: terrain material missing, using default material"));
		LoadedTerrainMaterial = UMaterial::GetDefaultMaterial(MD_Surface);
	}
	if (!LoadedWaterMaterial)
	{
		LoadedWaterMaterial = LoadedTerrainMaterial;
	}

	Completed = MakeShared<TQueue<FBuildResult, EQueueMode::Mpsc>, ESPMode::ThreadSafe>();
	Cancelled = MakeShared<std::atomic<bool>, ESPMode::ThreadSafe>(false);

	UE_LOG(LogUnrealTest, Log, TEXT("VoxelTerrain: %.0f m terrain, %.2f m voxels, %d LOD levels, root chunk %.1f m"),
		Field->GetSizeMeters(), BaseVoxelSize, LodLevels, ChunkSizeAt(0));
}

void AVoxelTerrain::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Cancelled)
	{
		*Cancelled = true;
	}
	for (TPair<FVoxelChunkKey, FChunk>& Pair : Chunks)
	{
		DestroyChunk(Pair.Value);
	}
	Chunks.Reset();
	Building.Reset();
	Super::EndPlay(EndPlayReason);
}

double AVoxelTerrain::VoxelSizeAt(int32 Level) const
{
	return BaseVoxelSize * double(1 << (LodLevels - 1 - Level));
}

bool AVoxelTerrain::GetCameraLocal(FVector& OutLocalMeters) const
{
	const APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PC || !PC->PlayerCameraManager)
	{
		return false;
	}
	OutLocalMeters = (PC->PlayerCameraManager->GetCameraLocation() - GetActorLocation()) * 0.01;
	return true;
}

float AVoxelTerrain::GetGroundHeightAt(const FVector& WorldLocation) const
{
	if (!Field)
	{
		return GetActorLocation().Z;
	}
	const FVector Local = (WorldLocation - GetActorLocation()) * 0.01;
	const double Size = Field->GetSizeMeters();
	const FVoxelColumnSample S = Field->Sample(FMath::Clamp(Local.X, 0.0, Size), FMath::Clamp(Local.Y, 0.0, Size));
	return float(GetActorLocation().Z + S.GroundHeight * 100.0);
}

void AVoxelTerrain::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!Field)
	{
		return;
	}

	ReceiveBuilds();

	TimeSinceUpdate += DeltaSeconds;
	FVector CameraLocal;
	if (TimeSinceUpdate >= UpdateInterval && GetCameraLocal(CameraLocal))
	{
		TimeSinceUpdate = 0.0f;
		UpdateDesired(CameraLocal);
		RemoveReplacedChunks();
	}

	StartBuilds();
}

void AVoxelTerrain::UpdateDesired(const FVector& CameraLocal)
{
	const double Size = Field->GetSizeMeters();
	const double Ground = Field->Sample(FMath::Clamp(CameraLocal.X, 0.0, Size), FMath::Clamp(CameraLocal.Y, 0.0, Size)).GroundHeight;
	const double HeightAboveGround = FMath::Max(0.0, CameraLocal.Z - Ground);

	Desired.Reset();
	const int32 Roots = FMath::CeilToInt(Size / ChunkSizeAt(0));
	for (int32 Y = 0; Y < Roots; ++Y)
	{
		for (int32 X = 0; X < Roots; ++X)
		{
			CollectDesired(CameraLocal, HeightAboveGround, 0, X, Y);
		}
	}

	// Nearest-first build order.
	Missing.Reset();
	for (const FVoxelChunkKey& Key : Desired)
	{
		if (!Chunks.Contains(Key) && !Building.Contains(Key))
		{
			Missing.Add(Key);
		}
	}
	const FVector2D Cam2D(CameraLocal.X, CameraLocal.Y);
	Missing.Sort([this, Cam2D](const FVoxelChunkKey& A, const FVoxelChunkKey& B)
	{
		const double SA = ChunkSizeAt(A.Level), SB = ChunkSizeAt(B.Level);
		return FVector2D::DistSquared(FVector2D((A.X + 0.5) * SA, (A.Y + 0.5) * SA), Cam2D)
			< FVector2D::DistSquared(FVector2D((B.X + 0.5) * SB, (B.Y + 0.5) * SB), Cam2D);
	});
}

void AVoxelTerrain::CollectDesired(const FVector& CameraLocal, double CameraHeightAboveGround, int32 Level, int32 X, int32 Y)
{
	const double Size = ChunkSizeAt(Level);
	const double MinX = X * Size, MinY = Y * Size;
	if (MinX >= Field->GetSizeMeters() || MinY >= Field->GetSizeMeters())
	{
		return;
	}

	const double DX = FMath::Max3(0.0, MinX - CameraLocal.X, CameraLocal.X - (MinX + Size));
	const double DY = FMath::Max3(0.0, MinY - CameraLocal.Y, CameraLocal.Y - (MinY + Size));
	const double Distance = FMath::Sqrt(DX * DX + DY * DY + CameraHeightAboveGround * CameraHeightAboveGround);

	if (Level < LodLevels - 1 && Distance < LodDistanceFactor * Size)
	{
		for (int32 Child = 0; Child < 4; ++Child)
		{
			CollectDesired(CameraLocal, CameraHeightAboveGround, Level + 1, X * 2 + (Child & 1), Y * 2 + (Child >> 1));
		}
	}
	else
	{
		Desired.Add({ Level, X, Y });
	}
}

void AVoxelTerrain::StartBuilds()
{
	while (Building.Num() < MaxConcurrentBuilds && Missing.Num() > 0)
	{
		const FVoxelChunkKey Key = Missing[0];
		Missing.RemoveAt(0);
		if (!Desired.Contains(Key) || Chunks.Contains(Key) || Building.Contains(Key))
		{
			continue;
		}

		FVoxelChunkBuildParams Params;
		Params.VoxelSize = VoxelSizeAt(Key.Level);
		Params.Columns = ColumnsPerChunk;
		Params.BaseVoxelSize = BaseVoxelSize;
		Params.bSmoothSurface = bSmoothSurface;
		Params.OriginMeters = FVector2D(Key.X, Key.Y) * ChunkSizeAt(Key.Level);
		Building.Add(Key);

		Async(EAsyncExecution::ThreadPool, [Field = Field, Completed = Completed, Cancelled = Cancelled, Params, Key]()
		{
			if (*Cancelled)
			{
				return;
			}
			TSharedPtr<FVoxelChunkMeshData, ESPMode::ThreadSafe> Data = MakeShared<FVoxelChunkMeshData, ESPMode::ThreadSafe>();
			FVoxelChunkMesher::Build(*Field, Params, *Data);
			Completed->Enqueue({ Key, Data });
		});
	}
}

void AVoxelTerrain::ReceiveBuilds()
{
	FBuildResult Result;
	int32 Uploaded = 0;
	while (Uploaded < MaxMeshUploadsPerTick && Completed->Dequeue(Result))
	{
		Building.Remove(Result.Key);
		if (!Desired.Contains(Result.Key) || Chunks.Contains(Result.Key))
		{
			continue;
		}

		const FVector RelativeLocation(FVector(FVector2D(Result.Key.X, Result.Key.Y) * ChunkSizeAt(Result.Key.Level) * 100.0, 0.0));
		FChunk Chunk;
		Chunk.Ground = CreateMeshComponent(Result.Data->Ground, LoadedTerrainMaterial, RelativeLocation, true);
		Chunk.Water = CreateMeshComponent(Result.Data->Water, LoadedWaterMaterial, RelativeLocation, false);
		Chunks.Add(Result.Key, Chunk);
		++Uploaded;
	}
}

bool AVoxelTerrain::Overlaps(const FVoxelChunkKey& A, const FVoxelChunkKey& B)
{
	const FVoxelChunkKey& Coarse = A.Level <= B.Level ? A : B;
	const FVoxelChunkKey& Fine = A.Level <= B.Level ? B : A;
	const int32 Shift = Fine.Level - Coarse.Level;
	return (Fine.X >> Shift) == Coarse.X && (Fine.Y >> Shift) == Coarse.Y;
}

void AVoxelTerrain::RemoveReplacedChunks()
{
	// A chunk that is no longer wanted stays visible until every desired chunk covering its area is ready.
	TArray<FVoxelChunkKey> Remove;
	for (const TPair<FVoxelChunkKey, FChunk>& Pair : Chunks)
	{
		if (Desired.Contains(Pair.Key))
		{
			continue;
		}
		bool bCovered = true;
		for (const FVoxelChunkKey& Wanted : Desired)
		{
			if (Overlaps(Wanted, Pair.Key) && !Chunks.Contains(Wanted))
			{
				bCovered = false;
				break;
			}
		}
		if (bCovered)
		{
			Remove.Add(Pair.Key);
		}
	}
	for (const FVoxelChunkKey& Key : Remove)
	{
		DestroyChunk(Chunks[Key]);
		Chunks.Remove(Key);
	}
}

UProceduralMeshComponent* AVoxelTerrain::CreateMeshComponent(const FVoxelMeshBuffers& Buffers, UMaterialInterface* Material, const FVector& RelativeLocation, bool bCastShadow)
{
	if (Buffers.IsEmpty())
	{
		return nullptr;
	}
	UProceduralMeshComponent* Mesh = NewObject<UProceduralMeshComponent>(this, NAME_None, RF_Transient);
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetRelativeLocation(RelativeLocation);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCastShadow(bCastShadow);
	Mesh->CreateMeshSection_LinearColor(0, Buffers.Vertices, Buffers.Triangles, Buffers.Normals, Buffers.UVs, Buffers.Colors, Buffers.Tangents, false);
	Mesh->SetMaterial(0, Material);
	Mesh->RegisterComponent();
	return Mesh;
}

void AVoxelTerrain::DestroyChunk(FChunk& Chunk)
{
	if (Chunk.Ground)
	{
		Chunk.Ground->DestroyComponent();
	}
	if (Chunk.Water)
	{
		Chunk.Water->DestroyComponent();
	}
	Chunk = FChunk();
}
