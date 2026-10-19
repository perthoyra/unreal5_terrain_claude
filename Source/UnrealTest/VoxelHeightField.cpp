#include "VoxelHeightField.h"
#include "Algo/Reverse.h"
#include "Dom/JsonObject.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
	constexpr float NoRiver = 1.0e6f;

	FVector2D CatmullRom(const FVector2D& P0, const FVector2D& P1, const FVector2D& P2, const FVector2D& P3, double T)
	{
		const double T2 = T * T;
		const double T3 = T2 * T;
		return 0.5 * ((2.0 * P1) + (P2 - P0) * T + (2.0 * P0 - 5.0 * P1 + 4.0 * P2 - P3) * T2 + (3.0 * P1 - P0 - 3.0 * P2 + P3) * T3);
	}
}

bool FVoxelHeightField::Load(const FString& RawFile, const FString& MetaFile, FString& OutError)
{
	FString MetaText;
	if (!FFileHelper::LoadFileToString(MetaText, *MetaFile))
	{
		OutError = FString::Printf(TEXT("Cannot read %s (run Tools/fetch_heightmap.py)"), *MetaFile);
		return false;
	}

	TSharedPtr<FJsonObject> Meta;
	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(MetaText), Meta) || !Meta.IsValid())
	{
		OutError = FString::Printf(TEXT("Invalid JSON in %s"), *MetaFile);
		return false;
	}

	Resolution = Meta->GetIntegerField(TEXT("resolution"));
	Spacing = Meta->GetNumberField(TEXT("spacing_m"));
	HeightRange = Meta->GetNumberField(TEXT("max_elevation_m")) - Meta->GetNumberField(TEXT("min_elevation_m"));

	TArray<uint8> Raw;
	if (!FFileHelper::LoadFileToArray(Raw, *RawFile) || Raw.Num() != Resolution * Resolution * 2)
	{
		OutError = FString::Printf(TEXT("Cannot read %s or size does not match %dx%d"), *RawFile, Resolution, Resolution);
		return false;
	}

	Heights.SetNumUninitialized(Resolution * Resolution);
	const double Scale = HeightRange / 65535.0;
	for (int32 Index = 0; Index < Heights.Num(); ++Index)
	{
		const uint16 Value = uint16(Raw[Index * 2]) | (uint16(Raw[Index * 2 + 1]) << 8);
		Heights[Index] = float(Value * Scale);
	}
	return true;
}

void FVoxelHeightField::BuildRiver(const FVoxelRiverSettings& Settings)
{
	River = Settings;
	bHasRiver = Settings.ControlPoints.Num() >= 2;
	if (!bHasRiver)
	{
		return;
	}

	// Sample the Catmull-Rom spline every ~0.5 m.
	TArray<FVector2D> Path;
	const TArray<FVector2D>& P = Settings.ControlPoints;
	for (int32 Seg = 0; Seg < P.Num() - 1; ++Seg)
	{
		const FVector2D& P0 = P[FMath::Max(Seg - 1, 0)];
		const FVector2D& P3 = P[FMath::Min(Seg + 2, P.Num() - 1)];
		const int32 Steps = FMath::Max(2, FMath::CeilToInt(FVector2D::Distance(P[Seg], P[Seg + 1]) / 0.5));
		for (int32 Step = 0; Step < Steps; ++Step)
		{
			Path.Add(CatmullRom(P0, P[Seg], P[Seg + 1], P3, double(Step) / Steps));
		}
	}
	Path.Add(P.Last());

	// Lowest ground across the channel at each sample.
	TArray<double> Floor;
	Floor.SetNum(Path.Num());
	for (int32 Index = 0; Index < Path.Num(); ++Index)
	{
		const FVector2D Dir = (Path[FMath::Min(Index + 1, Path.Num() - 1)] - Path[FMath::Max(Index - 1, 0)]).GetSafeNormal();
		const FVector2D Side(-Dir.Y, Dir.X);
		double Lowest = TNumericLimits<double>::Max();
		for (int32 K = -2; K <= 2; ++K)
		{
			const FVector2D Q = Path[Index] + Side * (Settings.HalfWidth * K / 2.0);
			Lowest = FMath::Min(Lowest, SampleBaseHeight(Q.X, Q.Y));
		}
		Floor[Index] = Lowest;
	}

	// Flow from the higher end to the lower end; the water level is a running minimum so it never climbs.
	if (Floor[0] < Floor.Last())
	{
		Algo::Reverse(Path);
		Algo::Reverse(Floor);
	}
	TArray<double> Level;
	Level.SetNum(Path.Num());
	double Running = TNumericLimits<double>::Max();
	for (int32 Index = 0; Index < Path.Num(); ++Index)
	{
		Running = FMath::Min(Running, Floor[Index] - 0.6);
		Level[Index] = Running;
	}

	// Limit the water slope: walking upstream from the mouth, lower any level that would fall faster than
	// MaxSlope towards the next sample. Levels only go down (never above the ground) and stay non-increasing,
	// so a sudden dip in the terrain becomes a long gentle descent instead of a rapid. The surface across the
	// channel takes the level of the nearest centreline sample, so a gentle slope also keeps it flat across.
	const double MaxSlope = 0.015;
	for (int32 Index = Path.Num() - 2; Index >= 0; --Index)
	{
		Level[Index] = FMath::Min(Level[Index], Level[Index + 1] + MaxSlope * FVector2D::Distance(Path[Index], Path[Index + 1]));
	}

	// Round off the kinks: a moving average over +-20 m (index clamped at the ends) of a non-increasing,
	// slope-limited sequence is still non-increasing and slope-limited. Banks and bed are shaped relative to the
	// level, so the water stays contained.
	const int32 HalfWindow = 40;
	TArray<double> Prefix;
	Prefix.SetNum(Path.Num() + 2 * HalfWindow + 1);
	Prefix[0] = 0.0;
	for (int32 K = 0; K < Prefix.Num() - 1; ++K)
	{
		Prefix[K + 1] = Prefix[K] + Level[FMath::Clamp(K - HalfWindow, 0, Path.Num() - 1)];
	}
	for (int32 Index = 0; Index < Path.Num(); ++Index)
	{
		Level[Index] = (Prefix[Index + 2 * HalfWindow + 1] - Prefix[Index]) / (2 * HalfWindow + 1);
	}

	// Stamp distance-to-centreline and water level into grids. The radius also covers the valley cut.
	RiverDistance.Init(NoRiver, Resolution * Resolution);
	RiverLevel.Init(0.0f, Resolution * Resolution);
	const double Radius = Settings.HalfWidth + Settings.BankWidth + 30.0;
	const int32 CellRadius = FMath::CeilToInt(Radius / Spacing) + 1;
	for (int32 Index = 0; Index < Path.Num(); ++Index)
	{
		const int32 CX = FMath::RoundToInt(Path[Index].X / Spacing);
		const int32 CY = FMath::RoundToInt(Path[Index].Y / Spacing);
		for (int32 Y = FMath::Max(CY - CellRadius, 0); Y <= FMath::Min(CY + CellRadius, Resolution - 1); ++Y)
		{
			for (int32 X = FMath::Max(CX - CellRadius, 0); X <= FMath::Min(CX + CellRadius, Resolution - 1); ++X)
			{
				const float Dist = float(FVector2D::Distance(FVector2D(X * Spacing, Y * Spacing), Path[Index]));
				const int32 Cell = Y * Resolution + X;
				if (Dist < RiverDistance[Cell])
				{
					RiverDistance[Cell] = Dist;
					RiverLevel[Cell] = float(Level[Index]);
				}
			}
		}
	}
}

double FVoxelHeightField::SampleGrid(const TArray<float>& Grid, double X, double Y) const
{
	const double GX = FMath::Clamp(X / Spacing, 0.0, double(Resolution - 1));
	const double GY = FMath::Clamp(Y / Spacing, 0.0, double(Resolution - 1));
	const int32 X0 = FMath::Min(int32(GX), Resolution - 2);
	const int32 Y0 = FMath::Min(int32(GY), Resolution - 2);
	const double TX = GX - X0;
	const double TY = GY - Y0;
	const float* Row0 = &Grid[Y0 * Resolution + X0];
	const float* Row1 = Row0 + Resolution;
	const double A = FMath::Lerp(double(Row0[0]), double(Row0[1]), TX);
	const double B = FMath::Lerp(double(Row1[0]), double(Row1[1]), TX);
	return FMath::Lerp(A, B, TY);
}

double FVoxelHeightField::SampleHeightCubic(double X, double Y) const
{
	const double GX = FMath::Clamp(X / Spacing, 0.0, double(Resolution - 1));
	const double GY = FMath::Clamp(Y / Spacing, 0.0, double(Resolution - 1));
	const int32 X0 = FMath::Min(FMath::FloorToInt(GX), Resolution - 2);
	const int32 Y0 = FMath::Min(FMath::FloorToInt(GY), Resolution - 2);

	auto Weights = [](double T, double W[4])
	{
		const double T2 = T * T, T3 = T2 * T;
		W[0] = -0.5 * T3 + T2 - 0.5 * T;
		W[1] = 1.5 * T3 - 2.5 * T2 + 1.0;
		W[2] = -1.5 * T3 + 2.0 * T2 + 0.5 * T;
		W[3] = 0.5 * T3 - 0.5 * T2;
	};
	double WX[4], WY[4];
	Weights(GX - X0, WX);
	Weights(GY - Y0, WY);

	double Sum = 0.0;
	for (int32 J = 0; J < 4; ++J)
	{
		const int32 Row = FMath::Clamp(Y0 - 1 + J, 0, Resolution - 1) * Resolution;
		double RowSum = 0.0;
		for (int32 I = 0; I < 4; ++I)
		{
			RowSum += WX[I] * Heights[Row + FMath::Clamp(X0 - 1 + I, 0, Resolution - 1)];
		}
		Sum += WY[J] * RowSum;
	}
	return Sum;
}

double FVoxelHeightField::SampleBaseHeight(double X, double Y) const
{
	double Height = SampleHeightCubic(X, Y);
	if (DetailNoiseAmplitude > 0.0)
	{
		// The DEM is ~4 m resolution; add small-scale relief so the 1 dm voxels have something to show.
		const double N = FMath::PerlinNoise2D(FVector2D(X / 9.0, Y / 9.0)) * 0.6
			+ FMath::PerlinNoise2D(FVector2D(X / 3.1 + 17.0, Y / 3.1 - 5.0)) * 0.3
			+ FMath::PerlinNoise2D(FVector2D(X / 1.2 - 41.0, Y / 1.2 + 9.0)) * 0.1;
		Height += N * DetailNoiseAmplitude;
	}
	return Height;
}

FVoxelColumnSample FVoxelHeightField::Sample(double X, double Y) const
{
	FVoxelColumnSample Out;
	Out.GroundHeight = SampleBaseHeight(X, Y);

	const double DX = SampleGrid(Heights, X + 1.0, Y) - SampleGrid(Heights, X - 1.0, Y);
	const double DY = SampleGrid(Heights, X, Y + 1.0) - SampleGrid(Heights, X, Y - 1.0);
	const double Slope = FMath::Sqrt(DX * DX + DY * DY) * 0.5;

	if (bHasRiver)
	{
		const double Dist = SampleGrid(RiverDistance, X, Y);
		if (Dist < River.HalfWidth + River.BankWidth + 30.0)
		{
			const double Level = SampleGrid(RiverLevel, X, Y);
			if (Dist < River.HalfWidth + River.BankWidth)
			{
				Out.bNearRiver = true;
				Out.WaterHeight = Level;
			}
			// The channel is the ground clamped between two curves that are continuous in Dist, so the bed,
			// shoreline, banks and valley sides blend into each other without steps:
			//   Upper: parabolic bed inside the channel, then valley sides rising at ~1:3.
			//   Lower: the bank crest (just above the water) that slopes away outside the banks.
			const double Upper = Dist <= River.HalfWidth
				? Level - 0.25 - River.Depth * (1.0 - FMath::Square(Dist / River.HalfWidth))
				: Level - 0.25 + (Dist - River.HalfWidth) * 0.35;
			const double Bank = Level + 0.2 - FMath::Max(0.0, Dist - River.HalfWidth - River.BankWidth) * 0.5;
			const double Lower = FMath::Min(Upper, Bank);
			Out.GroundHeight = FMath::Clamp(Out.GroundHeight, Lower, Upper);

			if (Out.GroundHeight < Level)
			{
				Out.WaterHeight = Level;
				Out.bHasWater = true;
				Out.bNearRiver = true;
				Out.Surface = EVoxelSurface::RiverBed;
				return Out;
			}
			if (Dist < River.HalfWidth + River.BankWidth + 2.0)
			{
				Out.Surface = EVoxelSurface::Sand;
				return Out;
			}
		}
	}

	if (Slope > 0.9)
	{
		Out.Surface = EVoxelSurface::Rock;
	}
	else if (Slope > 0.55 || FMath::PerlinNoise2D(FVector2D(X / 23.0 + 3.7, Y / 23.0 - 1.3)) > 0.35)
	{
		Out.Surface = EVoxelSurface::Dirt;
	}
	return Out;
}
