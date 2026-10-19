#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"

class FVoxelHeightField;

/** Vertex streams for one ProceduralMeshComponent section. */
struct FVoxelMeshBuffers
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FLinearColor> Colors;
	TArray<FProcMeshTangent> Tangents;

	bool IsEmpty() const { return Triangles.Num() == 0; }

	/** Adds a quad; the winding is chosen so the visible side faces Normal. */
	void AddQuad(const FVector& A, const FVector& B, const FVector& C, const FVector& D, const FVector& Normal,
		const FVector& Tangent, const FVector2D& UVA, const FVector2D& UVC, const FLinearColor& Color);
};

struct FVoxelChunkMeshData
{
	FVoxelMeshBuffers Ground;
	FVoxelMeshBuffers Water;
};

/** Parameters for one chunk; positions are terrain-local metres. */
struct FVoxelChunkBuildParams
{
	FVector2D OriginMeters = FVector2D::ZeroVector;
	double VoxelSize = 0.1;
	int32 Columns = 128;
	/** Size of the smallest voxel; all heights are quantised to multiples of this. */
	double BaseVoxelSize = 0.1;
	/** Bottom of the terrain block (walls at the outer terrain edge go down to here). */
	double BaseHeight = -5.0;
	/**
	 * true: continuous surface through the voxel corners (steps evened out, curved via bicubic heights and
	 * per-vertex normals). false: classic blocky voxel columns.
	 */
	bool bSmoothSurface = true;
};

/**
 * Turns a square patch of voxel columns into a mesh.
 * Blocky: one top face per column (merged along X where height and surface match) and vertical faces
 * wherever a neighbour column is lower.
 * Smooth: a vertex on every voxel corner with interpolated height and normal, two triangles per voxel.
 * In both modes chunk borders get skirts so neighbouring chunks of a different LOD never show gaps,
 * and the outer terrain edge is closed with walls down to BaseHeight.
 */
class FVoxelChunkMesher
{
public:
	static void Build(const FVoxelHeightField& Field, const FVoxelChunkBuildParams& Params, FVoxelChunkMeshData& Out);
};
