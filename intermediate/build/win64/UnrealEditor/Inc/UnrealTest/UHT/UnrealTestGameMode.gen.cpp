// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "UnrealTest/UnrealTestGameMode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeUnrealTestGameMode() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
UNREALTEST_API UClass* Z_Construct_UClass_AUnrealTestGameMode();
UNREALTEST_API UClass* Z_Construct_UClass_AUnrealTestGameMode_NoRegister();
UPackage* Z_Construct_UPackage__Script_UnrealTest();
// End Cross Module References

// Begin Class AUnrealTestGameMode
void AUnrealTestGameMode::StaticRegisterNativesAUnrealTestGameMode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AUnrealTestGameMode);
UClass* Z_Construct_UClass_AUnrealTestGameMode_NoRegister()
{
	return AUnrealTestGameMode::StaticClass();
}
struct Z_Construct_UClass_AUnrealTestGameMode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Uses the terrain camera pawn and makes sure the level contains a voxel terrain. */" },
#endif
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering HLOD WorldPartition DataLayers Transformation" },
		{ "IncludePath", "UnrealTestGameMode.h" },
		{ "ModuleRelativePath", "UnrealTestGameMode.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Uses the terrain camera pawn and makes sure the level contains a voxel terrain." },
#endif
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AUnrealTestGameMode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_AUnrealTestGameMode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AGameModeBase,
	(UObject* (*)())Z_Construct_UPackage__Script_UnrealTest,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_AUnrealTestGameMode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_AUnrealTestGameMode_Statics::ClassParams = {
	&AUnrealTestGameMode::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x009002ACu,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_AUnrealTestGameMode_Statics::Class_MetaDataParams), Z_Construct_UClass_AUnrealTestGameMode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_AUnrealTestGameMode()
{
	if (!Z_Registration_Info_UClass_AUnrealTestGameMode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AUnrealTestGameMode.OuterSingleton, Z_Construct_UClass_AUnrealTestGameMode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_AUnrealTestGameMode.OuterSingleton;
}
template<> UNREALTEST_API UClass* StaticClass<AUnrealTestGameMode>()
{
	return AUnrealTestGameMode::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(AUnrealTestGameMode);
AUnrealTestGameMode::~AUnrealTestGameMode() {}
// End Class AUnrealTestGameMode

// Begin Registration
struct Z_CompiledInDeferFile_FID_Temp_projects_unreal_test_Source_UnrealTest_UnrealTestGameMode_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_AUnrealTestGameMode, AUnrealTestGameMode::StaticClass, TEXT("AUnrealTestGameMode"), &Z_Registration_Info_UClass_AUnrealTestGameMode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AUnrealTestGameMode), 1797814635U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Temp_projects_unreal_test_Source_UnrealTest_UnrealTestGameMode_h_3881037016(TEXT("/Script/UnrealTest"),
	Z_CompiledInDeferFile_FID_Temp_projects_unreal_test_Source_UnrealTest_UnrealTestGameMode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Temp_projects_unreal_test_Source_UnrealTest_UnrealTestGameMode_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
