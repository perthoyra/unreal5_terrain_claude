// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "VoxelTerrain.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef UNREALTEST_VoxelTerrain_generated_h
#error "VoxelTerrain.generated.h already included, missing '#pragma once' in VoxelTerrain.h"
#endif
#define UNREALTEST_VoxelTerrain_generated_h

#define FID_Temp_projects_unreal_test_Source_UnrealTest_VoxelTerrain_h_37_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execGetGroundHeightAt);


#define FID_Temp_projects_unreal_test_Source_UnrealTest_VoxelTerrain_h_37_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAVoxelTerrain(); \
	friend struct Z_Construct_UClass_AVoxelTerrain_Statics; \
public: \
	DECLARE_CLASS(AVoxelTerrain, AActor, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/UnrealTest"), NO_API) \
	DECLARE_SERIALIZER(AVoxelTerrain)


#define FID_Temp_projects_unreal_test_Source_UnrealTest_VoxelTerrain_h_37_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	AVoxelTerrain(AVoxelTerrain&&); \
	AVoxelTerrain(const AVoxelTerrain&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AVoxelTerrain); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AVoxelTerrain); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(AVoxelTerrain) \
	NO_API virtual ~AVoxelTerrain();


#define FID_Temp_projects_unreal_test_Source_UnrealTest_VoxelTerrain_h_34_PROLOG
#define FID_Temp_projects_unreal_test_Source_UnrealTest_VoxelTerrain_h_37_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Temp_projects_unreal_test_Source_UnrealTest_VoxelTerrain_h_37_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Temp_projects_unreal_test_Source_UnrealTest_VoxelTerrain_h_37_INCLASS_NO_PURE_DECLS \
	FID_Temp_projects_unreal_test_Source_UnrealTest_VoxelTerrain_h_37_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> UNREALTEST_API UClass* StaticClass<class AVoxelTerrain>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Temp_projects_unreal_test_Source_UnrealTest_VoxelTerrain_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
