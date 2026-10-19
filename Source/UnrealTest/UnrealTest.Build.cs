using UnrealBuildTool;

public class UnrealTest : ModuleRules
{
	public UnrealTest(ReadOnlyTargetRules Target) : base(Target)
	{
		// UE 5.4 + MSVC 14.44+: <sanitizer/asan_interface.h> now exists, which makes ConcurrentLinearAllocator.h
		// evaluate the clang-only __has_feature (error C4668). Opting out of the ASan include avoids it; the
		// engine's shared PCH would not see this definition, so the module compiles without one.
		PCHUsage = PCHUsageMode.NoPCHs;
		PrivateDefinitions.Add("PLATFORM_HAS_ASAN_INCLUDE=0");

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"ProceduralMeshComponent"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Json"
		});
	}
}
