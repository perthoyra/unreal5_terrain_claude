#pragma once

#include "CoreMinimal.h"

/** Surface type of a voxel column; drives the vertex colour that tints the ground texture. */
enum class EVoxelSurface : uint8
{
	Grass,
	Dirt,
	Rock,
	Sand,
	RiverBed
};

/** River description in terrain-local metres (X = east, Y = south, origin = north-west corner). */
struct FVoxelRiverSettings
{
	TArray<FVector2D> ControlPoints;
	double HalfWidth = 5.0;
	double Depth = 1.5;
	double BankWidth = 3.0;
};

/** Result of sampling one voxel column. Heights are metres above the lowest terrain point. */
struct FVoxelColumnSample
{
	double GroundHeight = 0.0;
	double WaterHeight = 0.0;
	bool bHasWater = false;
	/** Inside the channel or its banks; WaterHeight is valid (the water plane is hidden under the banks). */
	bool bNearRiver = false;
	EVoxelSurface Surface = EVoxelSurface::Grass;
};

/**
 * Immutable terrain description shared by all chunk builders.
 * After Load() and BuildRiver() it is only read, so it is safe to sample from worker threads.
 */
class FVoxelHeightField
{
public:
	/** Loads heightmap.r16 + heightmap.json (written by Tools/fetch_heightmap.py). */
	bool Load(const FString& RawFile, const FString& MetaFile, FString& OutError);

	/** Rasterises the river into distance / water-level grids. Water level only ever decreases downstream. */
	void BuildRiver(const FVoxelRiverSettings& Settings);

	void SetDetailNoise(double Amplitude) { DetailNoiseAmplitude = Amplitude; }

	FVoxelColumnSample Sample(double X, double Y) const;

	double GetSizeMeters() const { return (Resolution - 1) * Spacing; }
	double GetMaxHeight() const { return HeightRange + DetailNoiseAmplitude; }
	bool IsInside(double X, double Y) const { return X >= 0.0 && Y >= 0.0 && X < GetSizeMeters() && Y < GetSizeMeters(); }

private:
	double SampleGrid(const TArray<float>& Grid, double X, double Y) const;
	/** Catmull-Rom (C1-continuous) height sample, so the surface curves instead of creasing at grid lines. */
	double SampleHeightCubic(double X, double Y) const;
	double SampleBaseHeight(double X, double Y) const;

	int32 Resolution = 0;
	double Spacing = 1.0;
	double HeightRange = 0.0;
	double DetailNoiseAmplitude = 0.25;

	TArray<float> Heights;
	TArray<float> RiverDistance;
	TArray<float> RiverLevel;
	FVoxelRiverSettings River;
	bool bHasRiver = false;
};
