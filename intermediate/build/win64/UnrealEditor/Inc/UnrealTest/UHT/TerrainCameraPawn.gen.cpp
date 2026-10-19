// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "UnrealTest/TerrainCameraPawn.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeTerrainCameraPawn() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_ADefaultPawn();
UNREALTEST_API UClass* Z_Construct_UClass_ATerrainCameraPawn();
UNREALTEST_API UClass* Z_Construct_UClass_ATerrainCameraPawn_NoRegister();
UPackage* Z_Construct_UPackage__Script_UnrealTest();
// End Cross Module References

// Begin Class ATerrainCameraPawn
void ATerrainCameraPawn::StaticRegisterNativesATerrainCameraPawn()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ATerrainCameraPawn);
UClass* Z_Construct_UClass_ATerrainCameraPawn_NoRegister()
{
	return ATerrainCameraPawn::StaticClass();
}
struct Z_Construct_UClass_ATerrainCameraPawn_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Free-flying camera with the standard Unreal controls:\n * WASD / arrows move, E / Space up, Q / C down, mouse look, gamepad sticks.\n * Additionally: mouse wheel changes speed, hold Left Shift to boost.\n */" },
#endif
		{ "HideCategories", "Navigation" },
		{ "IncludePath", "TerrainCameraPawn.h" },
		{ "ModuleRelativePath", "TerrainCameraPawn.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Free-flying camera with the standard Unreal controls:\nWASD / arrows move, E / Space up, Q / C down, mouse look, gamepad sticks.\nAdditionally: mouse wheel changes speed, hold Left Shift to boost." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BaseSpeed_MetaData[] = {
		{ "Category", "Camera" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Base flying speed in cm/s. */" },
#endif
		{ "ModuleRelativePath", "TerrainCameraPawn.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Base flying speed in cm/s." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_BoostMultiplier_MetaData[] = {
		{ "Category", "Camera" },
		{ "ModuleRelativePath", "TerrainCameraPawn.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bShowHelp_MetaData[] = {
		{ "Category", "Camera" },
		{ "ModuleRelativePath", "TerrainCameraPawn.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFloatPropertyParams NewProp_BaseSpeed;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_BoostMultiplier;
	static void NewProp_bShowHelp_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bShowHelp;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ATerrainCameraPawn>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_ATerrainCameraPawn_Statics::NewProp_BaseSpeed = { "BaseSpeed", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ATerrainCameraPawn, BaseSpeed), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BaseSpeed_MetaData), NewProp_BaseSpeed_MetaData) };
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_ATerrainCameraPawn_Statics::NewProp_BoostMultiplier = { "BoostMultiplier", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(ATerrainCameraPawn, BoostMultiplier), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_BoostMultiplier_MetaData), NewProp_BoostMultiplier_MetaData) };
void Z_Construct_UClass_ATerrainCameraPawn_Statics::NewProp_bShowHelp_SetBit(void* Obj)
{
	((ATerrainCameraPawn*)Obj)->bShowHelp = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_ATerrainCameraPawn_Statics::NewProp_bShowHelp = { "bShowHelp", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(ATerrainCameraPawn), &Z_Construct_UClass_ATerrainCameraPawn_Statics::NewProp_bShowHelp_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bShowHelp_MetaData), NewProp_bShowHelp_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_ATerrainCameraPawn_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ATerrainCameraPawn_Statics::NewProp_BaseSpeed,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ATerrainCameraPawn_Statics::NewProp_BoostMultiplier,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_ATerrainCameraPawn_Statics::NewProp_bShowHelp,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ATerrainCameraPawn_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_ATerrainCameraPawn_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_ADefaultPawn,
	(UObject* (*)())Z_Construct_UPackage__Script_UnrealTest,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ATerrainCameraPawn_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ATerrainCameraPawn_Statics::ClassParams = {
	&ATerrainCameraPawn::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_ATerrainCameraPawn_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_ATerrainCameraPawn_Statics::PropPointers),
	0,
	0x009000A4u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ATerrainCameraPawn_Statics::Class_MetaDataParams), Z_Construct_UClass_ATerrainCameraPawn_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ATerrainCameraPawn()
{
	if (!Z_Registration_Info_UClass_ATerrainCameraPawn.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ATerrainCameraPawn.OuterSingleton, Z_Construct_UClass_ATerrainCameraPawn_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ATerrainCameraPawn.OuterSingleton;
}
template<> UNREALTEST_API UClass* StaticClass<ATerrainCameraPawn>()
{
	return ATerrainCameraPawn::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(ATerrainCameraPawn);
ATerrainCameraPawn::~ATerrainCameraPawn() {}
// End Class ATerrainCameraPawn

// Begin Registration
struct Z_CompiledInDeferFile_FID_Temp_projects_unreal_test_Source_UnrealTest_TerrainCameraPawn_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ATerrainCameraPawn, ATerrainCameraPawn::StaticClass, TEXT("ATerrainCameraPawn"), &Z_Registration_Info_UClass_ATerrainCameraPawn, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ATerrainCameraPawn), 2062761533U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Temp_projects_unreal_test_Source_UnrealTest_TerrainCameraPawn_h_3474575317(TEXT("/Script/UnrealTest"),
	Z_CompiledInDeferFile_FID_Temp_projects_unreal_test_Source_UnrealTest_TerrainCameraPawn_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Temp_projects_unreal_test_Source_UnrealTest_TerrainCameraPawn_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
