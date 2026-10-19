// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "UnrealTest/VoxelTerrain.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeVoxelTerrain() {}

// Begin Cross Module References
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector2D();
ENGINE_API UClass* Z_Construct_UClass_AActor();
ENGINE_API UClass* Z_Construct_UClass_UMaterialInterface_NoRegister();
UNREALTEST_API UClass* Z_Construct_UClass_AVoxelTerrain();
UNREALTEST_API UClass* Z_Construct_UClass_AVoxelTerrain_NoRegister();
UPackage* Z_Construct_UPackage__Script_UnrealTest();
// End Cross Module References

// Begin Class AVoxelTerrain Function GetGroundHeightAt
struct Z_Construct_UFunction_AVoxelTerrain_GetGroundHeightAt_Statics
{
	struct VoxelTerrain_eventGetGroundHeightAt_Parms
	{
		FVector WorldLocation;
		float ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Voxel Terrain" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Ground height (world Z, cm) at a world location; returns the actor Z when the terrain is not loaded. */" },
#endif
		{ "ModuleRelativePath", "VoxelTerrain.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Ground height (world Z, cm) at a world location; returns the actor Z when the terrain is not loaded." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WorldLocation_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_WorldLocation;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_AVoxelTerrain_GetGroundHeightAt_Statics::NewProp_WorldLocation = { "WorldLocation", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(VoxelTerrain_eventGetGroundHeightAt_Parms, WorldLocation), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WorldLocation_MetaData), NewProp_WorldLocation_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_AVoxelTerrain_GetGroundHeightAt_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(VoxelTerrain_eventGetGroundHeightAt_Parms, ReturnValue), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AVoxelTerrain_GetGroundHeightAt_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AVoxelTerrain_GetGroundHeightAt_Statics::NewProp_WorldLocation,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AVoxelTerrain_GetGroundHeightAt_Statics::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_AVoxelTerrain_GetGroundHeightAt_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_AVoxelTerrain_GetGroundHeightAt_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AVoxelTerrain, nullptr, "GetGroundHeightAt", nullptr, nullptr, Z_Construct_UFunction_AVoxelTerrain_GetGroundHeightAt_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_AVoxelTerrain_GetGroundHeightAt_Statics::PropPointers), sizeof(Z_Construct_UFunction_AVoxelTerrain_GetGroundHeightAt_Statics::VoxelTerrain_eventGetGroundHeightAt_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54C20401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_AVoxelTerrain_GetGroundHeightAt_Statics::Function_MetaDataParams), Z_Construct_UFunction_AVoxelTerrain_GetGroundHeightAt_Statics::Function_MetaDataParams) };
static_assert(sizeof(Z_Construct_UFunction_AVoxelTerrain_GetGroundHeightAt_Statics::VoxelTerrain_eventGetGroundHeightAt_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_AVoxelTerrain_GetGroundHeightAt()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_AVoxelTerrain_GetGroundHeightAt_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(AVoxelTerrain::execGetGroundHeightAt)
{
	P_GET_STRUCT_REF(FVector,Z_Param_Out_WorldLocation);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(float*)Z_Param__Result=P_THIS->GetGroundHeightAt(Z_Param_Out_WorldLocation);
	P_NATIVE_END;
}
// End Class AVoxelTerrain Function GetGroundHeightAt

// Begin Class AVoxelTerrain
void AVoxelTerrain::StaticRegisterNativesAVoxelTerrain()
{
	UClass* Class = AVoxelTerrain::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "GetGroundHeightAt", &AVoxelTerrain::execGetGroundHeightAt },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AVoxelTerrain);
UClass* Z_Construct_UClass_AVoxelTerrain_NoRegister()
{
	return AVoxelTerrain::StaticClass();
}
struct Z_Construct_UClass_AVoxelTerrain_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Voxel terrain built from a heightmap. The terrain is a grid of 1 dm voxel columns, far too many\n * to mesh at once (20000 x 20000 for 2 km), so it is streamed as a quadtree of chunks around the\n * camera: every chunk has ColumnsPerChunk^2 columns, and each coarser level doubles the voxel size.\n * Chunks are meshed on worker threads and swapped in once their replacement area is complete.\n *\n * The actor location is the north-west corner of the terrain at the lowest elevation.\n */" },
#endif
		{ "IncludePath", "VoxelTerrain.h" },
		{ "ModuleRelativePath", "VoxelTerrain.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Voxel terrain built from a heightmap. The terrain is a grid of 1 dm voxel columns, far too many\nto mesh at once (20000 x 20000 for 2 km), so it is streamed as a quadtree of chunks around the\ncamera: every chunk has ColumnsPerChunk^2 columns, and each coarser level doubles the voxel size.\nChunks are meshed on worker threads and swapped in once their replacement area is complete.\n\nThe actor location is the north-west corner of the terrain at the lowest elevation." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_HeightmapFile_MetaData[] = {
		{ "Category", "Voxel Terrain|Source" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Heightmap files, relative to the project Content directory. */" },
#endif
		{ "ModuleRelativePath", "VoxelTerrain.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Heightmap files, relative to the project Content directory." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_HeightmapMetaFile_MetaData[] = {
		{ "Category", "Voxel Terrain|Source" },
		{ "ModuleRelativePath", "VoxelTerrain.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_TerrainMaterial_MetaData[] = {
		{ "Category", "Voxel Terrain|Rendering" },
		{ "ModuleRelativePath", "VoxelTerrain.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_WaterMaterial_MetaData[] = {
		{ "Category", "Voxel Terrain|Rendering" },
		{ "ModuleRelativePath", "VoxelTerrain.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BaseVoxelSize_MetaData[] = {
		{ "Category", "Voxel Terrain|Voxels" },
		{ "ClampMin", "0.01" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Edge length of the finest voxel, in metres. */" },
#endif
		{ "ModuleRelativePath", "VoxelTerrain.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Edge length of the finest voxel, in metres." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ColumnsPerChunk_MetaData[] = {
		{ "Category", "Voxel Terrain|Voxels" },
		{ "ClampMax", "256" },
		{ "ClampMin", "8" },
		{ "ModuleRelativePath", "VoxelTerrain.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bSmoothSurface_MetaData[] = {
		{ "Category", "Voxel Terrain|Voxels" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Even out the voxel steps: render a curved, continuous surface through the voxel corners instead of blocky columns. */" },
#endif
		{ "ModuleRelativePath", "VoxelTerrain.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Even out the voxel steps: render a curved, continuous surface through the voxel corners instead of blocky columns." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LodLevels_MetaData[] = {
		{ "Category", "Voxel Terrain|Voxels" },
		{ "ClampMax", "10" },
		{ "ClampMin", "1" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Number of quadtree levels; the root level uses voxels of BaseVoxelSize * 2^(LodLevels-1). */" },
#endif
		{ "ModuleRelativePath", "VoxelTerrain.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Number of quadtree levels; the root level uses voxels of BaseVoxelSize * 2^(LodLevels-1)." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LodDistanceFactor_MetaData[] = {
		{ "Category", "Voxel Terrain|Streaming" },
		{ "ClampMin", "0.5" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** A node is split when the camera is closer than this many node sizes. */" },
#endif
		{ "ModuleRelativePath", "VoxelTerrain.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "A node is split when the camera is closer than this many node sizes." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MaxConcurrentBuilds_MetaData[] = {
		{ "Category", "Voxel Terrain|Streaming" },
		{ "ClampMin", "1" },
		{ "ModuleRelativePath", "VoxelTerrain.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MaxMeshUploadsPerTick_MetaData[] = {
		{ "Category", "Voxel Terrain|Streaming" },
		{ "ClampMin", "1" },
		{ "ModuleRelativePath", "VoxelTerrain.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_UpdateInterval_MetaData[] = {
		{ "Category", "Voxel Terrain|Streaming" },
		{ "ClampMin", "0.0" },
		{ "ModuleRelativePath", "VoxelTerrain.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DetailNoiseAmplitude_MetaData[] = {
		{ "Category", "Voxel Terrain|Shape" },
		{ "ClampMin", "0.0" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Amplitude (m) of procedural micro relief added on top of the ~4 m DEM. */" },
#endif
		{ "ModuleRelativePath", "VoxelTerrain.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Amplitude (m) of procedural micro relief added on top of the ~4 m DEM." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_RiverControlPoints_MetaData[] = {
		{ "Category", "Voxel Terrain|River" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** River centreline in terrain-local metres (X = east, Y = south from the actor origin). */" },
#endif
		{ "ModuleRelativePath", "VoxelTerrain.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "River centreline in terrain-local metres (X = east, Y = south from the actor origin)." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_RiverHalfWidth_MetaData[] = {
		{ "Category", "Voxel Terrain|River" },
		{ "ClampMin", "0.5" },
		{ "ModuleRelativePath", "VoxelTerrain.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_RiverDepth_MetaData[] = {
		{ "Category", "Voxel Terrain|River" },
		{ "ClampMin", "0.1" },
		{ "ModuleRelativePath", "VoxelTerrain.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_RiverBankWidth_MetaData[] = {
		{ "Category", "Voxel Terrain|River" },
		{ "ClampMin", "0.0" },
		{ "ModuleRelativePath", "VoxelTerrain.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LoadedTerrainMaterial_MetaData[] = {
		{ "ModuleRelativePath", "VoxelTerrain.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LoadedWaterMaterial_MetaData[] = {
		{ "ModuleRelativePath", "VoxelTerrain.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStrPropertyParams NewProp_HeightmapFile;
	static const UECodeGen_Private::FStrPropertyParams NewProp_HeightmapMetaFile;
	static const UECodeGen_Private::FSoftObjectPropertyParams NewProp_TerrainMaterial;
	static const UECodeGen_Private::FSoftObjectPropertyParams NewProp_WaterMaterial;
	static const UECodeGen_Private::FDoublePropertyParams NewProp_BaseVoxelSize;
	static const UECodeGen_Private::FIntPropertyParams NewProp_ColumnsPerChunk;
	static void NewProp_bSmoothSurface_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bSmoothSurface;
	static const UECodeGen_Private::FIntPropertyParams NewProp_LodLevels;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_LodDistanceFactor;
	static const UECodeGen_Private::FIntPropertyParams NewProp_MaxConcurrentBuilds;
	static const UECodeGen_Private::FIntPropertyParams NewProp_MaxMeshUploadsPerTick;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_UpdateInterval;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_DetailNoiseAmplitude;
	static const UECodeGen_Private::FStructPropertyParams NewProp_RiverControlPoints_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_RiverControlPoints;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_RiverHalfWidth;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_RiverDepth;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_RiverBankWidth;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_LoadedTerrainMaterial;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_LoadedWaterMaterial;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_AVoxelTerrain_GetGroundHeightAt, "GetGroundHeightAt" }, // 3357531322
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AVoxelTerrain>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FStrPropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_HeightmapFile = { "HeightmapFile", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AVoxelTerrain, HeightmapFile), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_HeightmapFile_MetaData), NewProp_HeightmapFile_MetaData) };
const UECodeGen_Private::FStrPropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_HeightmapMetaFile = { "HeightmapMetaFile", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AVoxelTerrain, HeightmapMetaFile), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_HeightmapMetaFile_MetaData), NewProp_HeightmapMetaFile_MetaData) };
const UECodeGen_Private::FSoftObjectPropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_TerrainMaterial = { "TerrainMaterial", nullptr, (EPropertyFlags)0x0014000000000001, UECodeGen_Private::EPropertyGenFlags::SoftObject, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AVoxelTerrain, TerrainMaterial), Z_Construct_UClass_UMaterialInterface_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_TerrainMaterial_MetaData), NewProp_TerrainMaterial_MetaData) };
const UECodeGen_Private::FSoftObjectPropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_WaterMaterial = { "WaterMaterial", nullptr, (EPropertyFlags)0x0014000000000001, UECodeGen_Private::EPropertyGenFlags::SoftObject, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AVoxelTerrain, WaterMaterial), Z_Construct_UClass_UMaterialInterface_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_WaterMaterial_MetaData), NewProp_WaterMaterial_MetaData) };
const UECodeGen_Private::FDoublePropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_BaseVoxelSize = { "BaseVoxelSize", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Double, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AVoxelTerrain, BaseVoxelSize), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BaseVoxelSize_MetaData), NewProp_BaseVoxelSize_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_ColumnsPerChunk = { "ColumnsPerChunk", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AVoxelTerrain, ColumnsPerChunk), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ColumnsPerChunk_MetaData), NewProp_ColumnsPerChunk_MetaData) };
void Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_bSmoothSurface_SetBit(void* Obj)
{
	((AVoxelTerrain*)Obj)->bSmoothSurface = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_bSmoothSurface = { "bSmoothSurface", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(AVoxelTerrain), &Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_bSmoothSurface_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bSmoothSurface_MetaData), NewProp_bSmoothSurface_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_LodLevels = { "LodLevels", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AVoxelTerrain, LodLevels), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LodLevels_MetaData), NewProp_LodLevels_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_LodDistanceFactor = { "LodDistanceFactor", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AVoxelTerrain, LodDistanceFactor), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LodDistanceFactor_MetaData), NewProp_LodDistanceFactor_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_MaxConcurrentBuilds = { "MaxConcurrentBuilds", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AVoxelTerrain, MaxConcurrentBuilds), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MaxConcurrentBuilds_MetaData), NewProp_MaxConcurrentBuilds_MetaData) };
const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_MaxMeshUploadsPerTick = { "MaxMeshUploadsPerTick", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AVoxelTerrain, MaxMeshUploadsPerTick), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MaxMeshUploadsPerTick_MetaData), NewProp_MaxMeshUploadsPerTick_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_UpdateInterval = { "UpdateInterval", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AVoxelTerrain, UpdateInterval), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_UpdateInterval_MetaData), NewProp_UpdateInterval_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_DetailNoiseAmplitude = { "DetailNoiseAmplitude", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AVoxelTerrain, DetailNoiseAmplitude), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DetailNoiseAmplitude_MetaData), NewProp_DetailNoiseAmplitude_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_RiverControlPoints_Inner = { "RiverControlPoints", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FVector2D, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_RiverControlPoints = { "RiverControlPoints", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AVoxelTerrain, RiverControlPoints), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_RiverControlPoints_MetaData), NewProp_RiverControlPoints_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_RiverHalfWidth = { "RiverHalfWidth", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AVoxelTerrain, RiverHalfWidth), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_RiverHalfWidth_MetaData), NewProp_RiverHalfWidth_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_RiverDepth = { "RiverDepth", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AVoxelTerrain, RiverDepth), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_RiverDepth_MetaData), NewProp_RiverDepth_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_RiverBankWidth = { "RiverBankWidth", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AVoxelTerrain, RiverBankWidth), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_RiverBankWidth_MetaData), NewProp_RiverBankWidth_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_LoadedTerrainMaterial = { "LoadedTerrainMaterial", nullptr, (EPropertyFlags)0x0144000000002000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AVoxelTerrain, LoadedTerrainMaterial), Z_Construct_UClass_UMaterialInterface_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LoadedTerrainMaterial_MetaData), NewProp_LoadedTerrainMaterial_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_LoadedWaterMaterial = { "LoadedWaterMaterial", nullptr, (EPropertyFlags)0x0144000000002000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(AVoxelTerrain, LoadedWaterMaterial), Z_Construct_UClass_UMaterialInterface_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LoadedWaterMaterial_MetaData), NewProp_LoadedWaterMaterial_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AVoxelTerrain_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_HeightmapFile,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_HeightmapMetaFile,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_TerrainMaterial,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_WaterMaterial,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_BaseVoxelSize,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_ColumnsPerChunk,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_bSmoothSurface,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_LodLevels,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_LodDistanceFactor,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_MaxConcurrentBuilds,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_MaxMeshUploadsPerTick,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_UpdateInterval,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_DetailNoiseAmplitude,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_RiverControlPoints_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_RiverControlPoints,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_RiverHalfWidth,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_RiverDepth,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_RiverBankWidth,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_LoadedTerrainMaterial,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AVoxelTerrain_Statics::NewProp_LoadedWaterMaterial,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AVoxelTerrain_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_AVoxelTerrain_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AActor,
	(UObject* (*)())Z_Construct_UPackage__Script_UnrealTest,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AVoxelTerrain_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AVoxelTerrain_Statics::ClassParams = {
	&AVoxelTerrain::StaticClass,
	"Engine",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_AVoxelTerrain_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_AVoxelTerrain_Statics::PropPointers),
	0,
	0x009000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AVoxelTerrain_Statics::Class_MetaDataParams), Z_Construct_UClass_AVoxelTerrain_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AVoxelTerrain()
{
	if (!Z_Registration_Info_UClass_AVoxelTerrain.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AVoxelTerrain.OuterSingleton, Z_Construct_UClass_AVoxelTerrain_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AVoxelTerrain.OuterSingleton;
}
template<> UNREALTEST_API UClass* StaticClass<AVoxelTerrain>()
{
	return AVoxelTerrain::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AVoxelTerrain);
AVoxelTerrain::~AVoxelTerrain() {}
// End Class AVoxelTerrain

// Begin Registration
struct Z_CompiledInDeferFile_FID_Temp_projects_unreal_test_Source_UnrealTest_VoxelTerrain_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AVoxelTerrain, AVoxelTerrain::StaticClass, TEXT("AVoxelTerrain"), &Z_Registration_Info_UClass_AVoxelTerrain, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AVoxelTerrain), 2313431483U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Temp_projects_unreal_test_Source_UnrealTest_VoxelTerrain_h_4272208313(TEXT("/Script/UnrealTest"),
	Z_CompiledInDeferFile_FID_Temp_projects_unreal_test_Source_UnrealTest_VoxelTerrain_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Temp_projects_unreal_test_Source_UnrealTest_VoxelTerrain_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
