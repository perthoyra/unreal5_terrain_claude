#include "VoxelChunkMesher.h"
#include "VoxelHeightField.h"

namespace
{
	struct FColumn
	{
		int32 Ground = 0;
		int32 Water = 0;
		bool bValid = false;
		bool bWater = false;
		EVoxelSurface Surface = EVoxelSurface::Grass;
	};

	FLinearColor SurfaceColor(EVoxelSurface Surface)
	{
		switch (Surface)
		{
		case EVoxelSurface::Dirt:     return FLinearColor(0.36f, 0.25f, 0.14f);
		case EVoxelSurface::Rock:     return FLinearColor(0.42f, 0.41f, 0.39f);
		case EVoxelSurface::Sand:     return FLinearColor(0.66f, 0.58f, 0.42f);
		case EVoxelSurface::RiverBed: return FLinearColor(0.33f, 0.29f, 0.22f);
		default:                      return FLinearColor(0.20f, 0.34f, 0.09f);
		}
	}

	/** Brightness variation per global column so individual voxels stay distinguishable on walls. */
	float ColumnVariation(int64 X, int64 Y)
	{
		uint32 H = uint32(X * 73856093) ^ uint32(Y * 19349663);
		H ^= H >> 13;
		H *= 0x5bd1e995;
		H ^= H >> 15;
		return 0.92f + 0.16f * float(H & 0xFFFF) / 65535.0f;
	}

	/** Column edge (in column units) shared with the neighbour in direction Dir. */
	struct FSideDir
	{
		int32 DX, DY;
		FVector Normal;
		int32 E0X, E0Y, E1X, E1Y;
	};

	const FSideDir SideDirs[4] =
	{
		{ +1, 0, FVector(1, 0, 0),  1, 0, 1, 1 },
		{ -1, 0, FVector(-1, 0, 0), 0, 0, 0, 1 },
		{ 0, +1, FVector(0, 1, 0),  0, 1, 1, 1 },
		{ 0, -1, FVector(0, -1, 0), 0, 0, 1, 0 },
	};
}

void FVoxelMeshBuffers::AddQuad(const FVector& A, const FVector& B, const FVector& C, const FVector& D, const FVector& Normal,
	const FVector& Tangent, const FVector2D& UVA, const FVector2D& UVC, const FLinearColor& Color)
{
	const int32 Base = Vertices.Num();
	Vertices.Append({ A, B, C, D });
	UVs.Append({ UVA, FVector2D(UVC.X, UVA.Y), UVC, FVector2D(UVA.X, UVC.Y) });
	const FProcMeshTangent PT(Tangent, false);
	for (int32 K = 0; K < 4; ++K)
	{
		Normals.Add(Normal);
		Colors.Add(Color);
		Tangents.Add(PT);
	}

	// Unreal (left-handed) treats a triangle as front-facing away from (V1 - V0) x (V2 - V0).
	if (FVector::DotProduct(FVector::CrossProduct(B - A, C - A), Normal) < 0.0)
	{
		Triangles.Append({ Base, Base + 1, Base + 2, Base, Base + 2, Base + 3 });
	}
	else
	{
		Triangles.Append({ Base, Base + 2, Base + 1, Base, Base + 3, Base + 2 });
	}
}

namespace
{
	/** The ground texture repeats every 2 m. */
	constexpr double UVPerMeter = 0.5;

	/**
	 * UV of the chunk origin, reduced to its fractional part. Mesh UVs are stored at reduced precision, so
	 * absolute world UVs (up to ~1000) would snap to 0.5 steps and sample one texel per metre; chunk-local
	 * UVs plus this offset stay small and still tile seamlessly across chunks.
	 */
	FVector2D UVOrigin(const FVector2D& OriginMeters)
	{
		return FVector2D(FMath::Frac(OriginMeters.X * UVPerMeter), FMath::Frac(OriginMeters.Y * UVPerMeter));
	}

	/** Adds an upward-facing triangle, fixing the winding (front face is opposite to (V1-V0) x (V2-V0)). */
	void AddTriangleUp(FVoxelMeshBuffers& B, int32 I0, int32 I1, int32 I2)
	{
		const FVector& A = B.Vertices[I0];
		if (FVector::CrossProduct(B.Vertices[I1] - A, B.Vertices[I2] - A).Z < 0.0)
		{
			B.Triangles.Append({ I0, I1, I2 });
		}
		else
		{
			B.Triangles.Append({ I0, I2, I1 });
		}
	}

	struct FCorner
	{
		double Ground = 0.0;
		double Water = 0.0;
		bool bValid = false;
		bool bNearRiver = false;
		EVoxelSurface Surface = EVoxelSurface::Grass;
	};

	/**
	 * Smooth surface: the voxel steps are evened out by placing a vertex on every voxel corner at the
	 * interpolated (bicubic) height instead of the quantised column height. Per-vertex normals from
	 * central differences make the shading curve continuously across voxels.
	 */
	void BuildSmooth(const FVoxelHeightField& Field, const FVoxelChunkBuildParams& Params, FVoxelChunkMeshData& Out)
	{
		constexpr double Eps = 1.0e-6;
		const int32 N = Params.Columns;
		const int32 Stride = N + 3; // corners -1 .. N+1, the outer ring is only used for normals
		const double V = Params.VoxelSize;
		const double Size = Field.GetSizeMeters();
		const double SkirtDepth = FMath::Max(0.5, 2.0 * V);
		const FVector2D Origin = Params.OriginMeters;

		TArray<FCorner> Corners;
		Corners.SetNum(Stride * Stride);
		auto At = [&Corners, Stride](int32 I, int32 J) -> FCorner& { return Corners[(J + 1) * Stride + (I + 1)]; };

		for (int32 J = -1; J <= N + 1; ++J)
		{
			for (int32 I = -1; I <= N + 1; ++I)
			{
				const double X = Origin.X + I * V;
				const double Y = Origin.Y + J * V;
				FCorner& C = At(I, J);
				C.bValid = X >= -Eps && Y >= -Eps && X <= Size + Eps && Y <= Size + Eps;
				const FVoxelColumnSample S = Field.Sample(FMath::Clamp(X, 0.0, Size), FMath::Clamp(Y, 0.0, Size));
				C.Ground = S.GroundHeight;
				C.Surface = S.Surface;
				C.bNearRiver = S.bNearRiver;
				C.Water = S.WaterHeight;
			}
		}

		const int32 GridSide = N + 1;
		const FVector2D UV0 = UVOrigin(Origin);
		auto UVAt = [UV0, V](int32 I, int32 J) { return UV0 + FVector2D(I * V, J * V) * UVPerMeter; };

		// Ground surface.
		TArray<int32> GroundIndex;
		GroundIndex.Init(INDEX_NONE, GridSide * GridSide);
		FVoxelMeshBuffers& G = Out.Ground;
		G.Vertices.Reserve(GridSide * GridSide);
		for (int32 J = 0; J <= N; ++J)
		{
			for (int32 I = 0; I <= N; ++I)
			{
				const FCorner& C = At(I, J);
				if (!C.bValid)
				{
					continue;
				}
				const double DX = (At(I + 1, J).Ground - At(I - 1, J).Ground) / (2.0 * V);
				const double DY = (At(I, J + 1).Ground - At(I, J - 1).Ground) / (2.0 * V);
				GroundIndex[J * GridSide + I] = G.Vertices.Num();
				G.Vertices.Add(FVector(I * V * 100.0, J * V * 100.0, C.Ground * 100.0));
				G.Normals.Add(FVector(-DX, -DY, 1.0).GetSafeNormal());
				G.Tangents.Add(FProcMeshTangent(FVector(1.0, 0.0, DX).GetSafeNormal(), false));
				G.UVs.Add(UVAt(I, J));
				G.Colors.Add(SurfaceColor(C.Surface));
			}
		}
		for (int32 J = 0; J < N; ++J)
		{
			for (int32 I = 0; I < N; ++I)
			{
				const int32 V00 = GroundIndex[J * GridSide + I], V10 = GroundIndex[J * GridSide + I + 1];
				const int32 V01 = GroundIndex[(J + 1) * GridSide + I], V11 = GroundIndex[(J + 1) * GridSide + I + 1];
				if (V00 == INDEX_NONE || V10 == INDEX_NONE || V01 == INDEX_NONE || V11 == INDEX_NONE)
				{
					continue;
				}
				// Split along the diagonal with the smaller height difference: follows ridges and valleys.
				const double D0 = FMath::Abs(G.Vertices[V00].Z - G.Vertices[V11].Z);
				const double D1 = FMath::Abs(G.Vertices[V10].Z - G.Vertices[V01].Z);
				if (D0 <= D1)
				{
					AddTriangleUp(G, V00, V10, V11);
					AddTriangleUp(G, V00, V11, V01);
				}
				else
				{
					AddTriangleUp(G, V00, V10, V01);
					AddTriangleUp(G, V10, V11, V01);
				}
			}
		}

		// Water surface over the channel and banks (the banks rise above it and hide its edges).
		TArray<int32> WaterIndex;
		WaterIndex.Init(INDEX_NONE, GridSide * GridSide);
		FVoxelMeshBuffers& W = Out.Water;
		for (int32 J = 0; J <= N; ++J)
		{
			for (int32 I = 0; I <= N; ++I)
			{
				const FCorner& C = At(I, J);
				if (!C.bValid || !C.bNearRiver)
				{
					continue;
				}
				WaterIndex[J * GridSide + I] = W.Vertices.Num();
				W.Vertices.Add(FVector(I * V * 100.0, J * V * 100.0, C.Water * 100.0));
				W.Normals.Add(FVector::UpVector);
				W.Tangents.Add(FProcMeshTangent(FVector::ForwardVector, false));
				W.UVs.Add(UVAt(I, J));
				W.Colors.Add(FLinearColor::White);
			}
		}
		for (int32 J = 0; J < N; ++J)
		{
			for (int32 I = 0; I < N; ++I)
			{
				const int32 V00 = WaterIndex[J * GridSide + I], V10 = WaterIndex[J * GridSide + I + 1];
				const int32 V01 = WaterIndex[(J + 1) * GridSide + I], V11 = WaterIndex[(J + 1) * GridSide + I + 1];
				if (V00 != INDEX_NONE && V10 != INDEX_NONE && V01 != INDEX_NONE && V11 != INDEX_NONE)
				{
					AddTriangleUp(W, V00, V10, V11);
					AddTriangleUp(W, V00, V11, V01);
				}
			}
		}

		// Skirts on chunk borders, closed walls on the outer terrain edge.
		const FLinearColor SideColor = SurfaceColor(EVoxelSurface::Dirt) * 0.9f;
		auto AddEdge = [&](int32 IA, int32 JA, int32 IB, int32 JB, const FVector& Normal, bool bTerrainEdge)
		{
			const FCorner& A = At(IA, JA);
			const FCorner& B = At(IB, JB);
			if (!A.bValid || !B.bValid)
			{
				return;
			}
			const FVector PA(IA * V * 100.0, JA * V * 100.0, 0.0);
			const FVector PB(IB * V * 100.0, JB * V * 100.0, 0.0);
			const FVector Tangent = (PB - PA).GetSafeNormal();
			const double LowA = bTerrainEdge ? Params.BaseHeight : A.Ground - SkirtDepth;
			const double LowB = bTerrainEdge ? Params.BaseHeight : B.Ground - SkirtDepth;
			const double Along0 = FVector::DotProduct(PA, Tangent) * 0.01 * UVPerMeter + FVector::DotProduct(FVector(UV0, 0.0), Tangent);
			const double Along1 = Along0 + V * UVPerMeter;
			const double ZOffset = FMath::FloorToDouble(A.Ground * UVPerMeter);
			G.AddQuad(PA + FVector(0, 0, LowA * 100.0), PB + FVector(0, 0, LowB * 100.0), PB + FVector(0, 0, B.Ground * 100.0), PA + FVector(0, 0, A.Ground * 100.0),
				Normal, Tangent, FVector2D(Along0, ZOffset - LowA * UVPerMeter), FVector2D(Along1, ZOffset - A.Ground * UVPerMeter), SideColor);

			if (bTerrainEdge && A.bNearRiver && B.bNearRiver && (A.Water > A.Ground || B.Water > B.Ground))
			{
				const double TopA = FMath::Max(A.Water, A.Ground), TopB = FMath::Max(B.Water, B.Ground);
				W.AddQuad(PA + FVector(0, 0, A.Ground * 100.0), PB + FVector(0, 0, B.Ground * 100.0), PB + FVector(0, 0, TopB * 100.0), PA + FVector(0, 0, TopA * 100.0),
					Normal, Tangent, FVector2D(Along0, 0.0), FVector2D(Along1, 1.0), FLinearColor::White);
			}
		};

		// The terrain edge always lies on a corner line, possibly inside the chunk.
		const int32 LastI = FMath::Min(N, FMath::RoundToInt((Size - Origin.X) / V));
		const int32 LastJ = FMath::Min(N, FMath::RoundToInt((Size - Origin.Y) / V));
		const bool bWestEdge = Origin.X <= Eps, bNorthEdge = Origin.Y <= Eps;
		const bool bEastEdge = Origin.X + LastI * V >= Size - Eps, bSouthEdge = Origin.Y + LastJ * V >= Size - Eps;
		for (int32 K = 0; K < N; ++K)
		{
			AddEdge(K, 0, K + 1, 0, FVector(0, -1, 0), bNorthEdge);
			AddEdge(K, LastJ, K + 1, LastJ, FVector(0, 1, 0), bSouthEdge);
			AddEdge(0, K, 0, K + 1, FVector(-1, 0, 0), bWestEdge);
			AddEdge(LastI, K, LastI, K + 1, FVector(1, 0, 0), bEastEdge);
		}
	}
}

void FVoxelChunkMesher::Build(const FVoxelHeightField& Field, const FVoxelChunkBuildParams& Params, FVoxelChunkMeshData& Out)
{
	if (Params.bSmoothSurface)
	{
		BuildSmooth(Field, Params, Out);
		return;
	}

	const int32 N = Params.Columns;
	const int32 Stride = N + 2;
	const double V = Params.VoxelSize;
	const int32 Step = FMath::Max(1, FMath::RoundToInt(V / Params.BaseVoxelSize));
	const double CmPerUnit = Params.BaseVoxelSize * 100.0;
	const double CmPerColumn = V * 100.0;
	const int32 BaseUnits = FMath::FloorToInt(Params.BaseHeight / Params.BaseVoxelSize);
	const int32 Skirt = FMath::Max(4 * Step, 10);
	const int64 GlobalX0 = FMath::RoundToInt64(Params.OriginMeters.X / V);
	const int64 GlobalY0 = FMath::RoundToInt64(Params.OriginMeters.Y / V);

	auto Quantise = [V, Step](double Height) { return int32(FMath::RoundToDouble(Height / V)) * Step; };

	// Sample one extra ring so border faces know their neighbours.
	TArray<FColumn> Cols;
	Cols.SetNum(Stride * Stride);
	auto Col = [&Cols, Stride](int32 I, int32 J) -> FColumn& { return Cols[(J + 1) * Stride + (I + 1)]; };

	for (int32 J = -1; J <= N; ++J)
	{
		for (int32 I = -1; I <= N; ++I)
		{
			const double X = Params.OriginMeters.X + (I + 0.5) * V;
			const double Y = Params.OriginMeters.Y + (J + 0.5) * V;
			FColumn& C = Col(I, J);
			if (!Field.IsInside(X, Y))
			{
				continue;
			}
			const FVoxelColumnSample S = Field.Sample(X, Y);
			C.bValid = true;
			C.Ground = Quantise(S.GroundHeight);
			C.Surface = S.Surface;
			if (S.bHasWater)
			{
				C.Water = Quantise(S.WaterHeight);
				C.bWater = C.Water > C.Ground;
			}
		}
	}

	Out.Ground.Vertices.Reserve(N * N * 2);
	auto UVOf = [](double LocalCm) { return LocalCm * 0.01 * UVPerMeter; };
	const FVector2D ChunkUV = UVOrigin(Params.OriginMeters);

	// Top faces, merged along X.
	for (int32 J = 0; J < N; ++J)
	{
		for (int32 I = 0; I < N;)
		{
			const FColumn& C = Col(I, J);
			if (!C.bValid)
			{
				++I;
				continue;
			}
			int32 End = I + 1;
			while (End < N && Col(End, J).bValid && Col(End, J).Ground == C.Ground && Col(End, J).Surface == C.Surface)
			{
				++End;
			}
			const double X0 = I * CmPerColumn, X1 = End * CmPerColumn;
			const double Y0 = J * CmPerColumn, Y1 = (J + 1) * CmPerColumn;
			const double Z = C.Ground * CmPerUnit;
			const FVector2D UV0 = ChunkUV + FVector2D(UVOf(X0), UVOf(Y0));
			const FVector2D UV1 = ChunkUV + FVector2D(UVOf(X1), UVOf(Y1));
			Out.Ground.AddQuad(FVector(X0, Y0, Z), FVector(X1, Y0, Z), FVector(X1, Y1, Z), FVector(X0, Y1, Z),
				FVector::UpVector, FVector::ForwardVector, UV0, UV1, SurfaceColor(C.Surface));
			I = End;
		}
	}

	// Water surface, merged along X.
	for (int32 J = 0; J < N; ++J)
	{
		for (int32 I = 0; I < N;)
		{
			const FColumn& C = Col(I, J);
			if (!C.bValid || !C.bWater)
			{
				++I;
				continue;
			}
			int32 End = I + 1;
			while (End < N && Col(End, J).bValid && Col(End, J).bWater && Col(End, J).Water == C.Water)
			{
				++End;
			}
			const double X0 = I * CmPerColumn, X1 = End * CmPerColumn;
			const double Y0 = J * CmPerColumn, Y1 = (J + 1) * CmPerColumn;
			const double Z = C.Water * CmPerUnit;
			const FVector2D UV0 = ChunkUV + FVector2D(UVOf(X0), UVOf(Y0));
			const FVector2D UV1 = ChunkUV + FVector2D(UVOf(X1), UVOf(Y1));
			Out.Water.AddQuad(FVector(X0, Y0, Z), FVector(X1, Y0, Z), FVector(X1, Y1, Z), FVector(X0, Y1, Z),
				FVector::UpVector, FVector::ForwardVector, UV0, UV1, FLinearColor::White);
			I = End;
		}
	}

	// Vertical faces wherever the neighbouring column is lower.
	for (int32 J = 0; J < N; ++J)
	{
		for (int32 I = 0; I < N; ++I)
		{
			const FColumn& C = Col(I, J);
			if (!C.bValid)
			{
				continue;
			}
			const FLinearColor SideColor = (C.Surface == EVoxelSurface::Grass ? SurfaceColor(EVoxelSurface::Dirt) : SurfaceColor(C.Surface))
				* ColumnVariation(GlobalX0 + I, GlobalY0 + J) * 0.9f;

			for (const FSideDir& D : SideDirs)
			{
				const int32 NI = I + D.DX, NJ = J + D.DY;
				const FColumn& Nb = Col(NI, NJ);
				const bool bChunkBorder = NI < 0 || NJ < 0 || NI >= N || NJ >= N;

				int32 Low = Nb.bValid ? Nb.Ground : BaseUnits;
				if (bChunkBorder && Nb.bValid)
				{
					Low = FMath::Min(Low, C.Ground - Skirt);
				}

				const FVector E0((I + D.E0X) * CmPerColumn, (J + D.E0Y) * CmPerColumn, 0.0);
				const FVector E1((I + D.E1X) * CmPerColumn, (J + D.E1Y) * CmPerColumn, 0.0);
				const FVector Tangent = (E1 - E0).GetSafeNormal();
				const double Along0 = UVOf(FVector::DotProduct(E0, Tangent)) + FVector::DotProduct(FVector(ChunkUV, 0.0), Tangent);
				const double Along1 = Along0 + UVOf(CmPerColumn);

				auto AddWall = [&](FVoxelMeshBuffers& Buffers, int32 Bottom, int32 Top, const FLinearColor& Color)
				{
					const double ZLow = Bottom * CmPerUnit, ZHigh = Top * CmPerUnit;
					const double ZOffset = FMath::FloorToDouble(UVOf(ZHigh));
					Buffers.AddQuad(E0 + FVector(0, 0, ZLow), E1 + FVector(0, 0, ZLow), E1 + FVector(0, 0, ZHigh), E0 + FVector(0, 0, ZHigh),
						D.Normal, Tangent, FVector2D(Along0, ZOffset - UVOf(ZLow)), FVector2D(Along1, ZOffset - UVOf(ZHigh)), Color);
				};

				if (Low < C.Ground)
				{
					AddWall(Out.Ground, Low, C.Ground, SideColor);
				}

				// Water side faces where the neighbour holds no water (terrain edge or a low bank voxel).
				if (C.bWater && !bChunkBorder && !(Nb.bValid && Nb.bWater))
				{
					const int32 WaterLow = FMath::Max(Nb.bValid ? Nb.Ground : C.Ground, C.Ground);
					if (WaterLow < C.Water)
					{
						AddWall(Out.Water, WaterLow, C.Water, FLinearColor::White);
					}
				}
				else if (C.bWater && bChunkBorder && !Nb.bValid)
				{
					AddWall(Out.Water, C.Ground, C.Water, FLinearColor::White);
				}
			}
		}
	}
}
