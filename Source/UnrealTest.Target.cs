using UnrealBuildTool;

public class UnrealTestTarget : TargetRules
{
	public UnrealTestTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		ExtraModuleNames.Add("UnrealTest");

		// MSVC 14.5x (Visual Studio 18) is too new for UE 5.4; use the VS 2022 toolchain.
		WindowsPlatform.CompilerVersion = "14.44.35207";
	}
}
