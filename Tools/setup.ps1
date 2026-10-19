# Rebuilds everything that is generated: heightmap, ground texture, C++ binaries, materials and map.
# Usage:  powershell -ExecutionPolicy Bypass -File Tools\setup.ps1 [-SkipHeightmap]
param(
    [string]$EngineDir = "E:\EpicGames\UE_5.4",
    [switch]$SkipHeightmap
)
$ErrorActionPreference = "Stop"
$Project = Join-Path (Split-Path $PSScriptRoot -Parent) "UnrealTest.uproject"

if (-not $SkipHeightmap) {
    Write-Host "== Heightmap (downloads public AWS Terrain Tiles)"
    python -I (Join-Path $PSScriptRoot "fetch_heightmap.py")
    if ($LASTEXITCODE -ne 0) { throw "fetch_heightmap.py failed" }
}

Write-Host "== Ground texture"
python -I (Join-Path $PSScriptRoot "make_ground_texture.py")
if ($LASTEXITCODE -ne 0) { throw "make_ground_texture.py failed" }

Write-Host "== Compile UnrealTestEditor"
& "$EngineDir\Engine\Build\BatchFiles\Build.bat" UnrealTestEditor Win64 Development "-Project=$Project" -WaitMutex
if ($LASTEXITCODE -ne 0) { throw "Build failed" }

Write-Host "== Create materials and map"
& "$EngineDir\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "$Project" -run=pythonscript "-script=$(Join-Path $PSScriptRoot 'setup_content.py')" -AllowCommandletRendering -unattended -nosplash -nop4 -NoSound
if ($LASTEXITCODE -ne 0) { throw "setup_content.py failed" }

Write-Host "Done. Open $Project and press Play."
