# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

- Include a dependency diagram. When updating this file, keep the package import diagram under Architecture, and redraw it whenever a package is added or removed or the imports between packages change.
- Always update #Architecture when claude.md is updated.
- Always include naming conventions.
- Always run unit tests if they exist.
- Never make changes which breaks the unit tests.
- Never implement changes when asked to show an example or when asked to make a suggestion.
- Always ask for permission before adding new packages, files, directories.
- Always ask for permission before modifying / moving code unless instructed to so.
- Always ask for permission before removing packages or files, even when they look unused. 
- Overrule the rule about asking for write permission if something has to be written to the e:\temp\projects\unreal_test folder.
- Never modify any test cases, suggest changes but never modify them.
- Never change files you see are manually edited and maintained, like rest.http ( exception being claude.md ).
- Always use open source and free of charge solutions if a 3rd party tool, framework or code module is needed.
- Do not send any personal information.
- Do not send any Telemetry.
- Do not create unit tests.
- Once complete then do "/init" if calude.md does not exist, else update the existing file.

# Project

Unreal Engine 5.4 C++ project (`UnrealTest.uproject`): a 2 km x 2 km voxel terrain with 1 dm (0.1 m) voxels,
built from a real DEM with a single mountain (SP Crater, Arizona), a basic ground texture, a river in the
south-east corner and the standard free-flying camera controls.

Third-party sources (rule: free / open only): Unreal Engine built-in plugins (ProceduralMeshComponent,
PythonScriptPlugin, EditorScriptingUtilities) and AWS Terrain Tiles (public open data, no account).

## Build and run

- Engine: `E:\EpicGames\UE_5.4`. Toolchain is pinned to MSVC 14.44 (VS 2022) in both `*.Target.cs`;
  the Visual Studio 18 toolchain (14.5x) is too new for UE 5.4.
- Full regeneration: `powershell -ExecutionPolicy Bypass -File Tools\setup.ps1` (heightmap download, texture,
  compile, materials + map). `-SkipHeightmap` reuses `Content/Terrain/heightmap.*`.
- Compile only: `E:\EpicGames\UE_5.4\Engine\Build\BatchFiles\Build.bat UnrealTestEditor Win64 Development -Project=<path>\UnrealTest.uproject -WaitMutex`
- Run standalone: `UnrealEditor.exe UnrealTest.uproject -game -windowed`, or open the project and press Play.
  Terrain only exists at runtime (it streams around the player camera); the editor viewport shows lights only.
- Verification hooks (pawn command-line options):
  `-AutoScreenshot=<s>` saves `Saved/Screenshots/AutoShot.png` once streaming and shader compilation are idle, then quits;
  `-CameraPose=X,Y,Height,Pitch,Yaw` (terrain-local metres, height above ground).
- No unit tests exist. To verify a change, recompile and run e.g.
  `UnrealEditor.exe UnrealTest.uproject -game -windowed -ResX=1280 -ResY=720 -NoLiveCoding -AutoScreenshot=8 -CameraPose=1700,1330,6,-15,125`
  (river view), then inspect `Saved/Screenshots/AutoShot.png`; runtime log goes to `Saved/Logs/` (category `LogUnrealTest`).
  The first run after material changes compiles shaders and takes a few minutes.
- After changing `Tools/setup_content.py`, re-run only its step (it deletes and recreates its assets and the map).
- IDE squiggles about missing `CoreMinimal.h` are expected; no IDE project files are generated (UBT is the source of truth).

## Controls

WASD / arrows move, E / Space up, Q / C down, mouse look (standard `ADefaultPawn` bindings),
mouse wheel changes speed, hold Left Shift to boost. Gamepad sticks/triggers also work.

# Architecture

The terrain is a heightfield of voxel columns: every 0.1 m x 0.1 m column is solid up to a height quantised
to 0.1 m. 2 km at 0.1 m is 20000 x 20000 columns, so it is never meshed at once. `AVoxelTerrain` streams a
quadtree of chunks around the camera; each chunk is 128 x 128 columns and each coarser level doubles the voxel
size (0.1 m near the camera, up to 3.2 m for 409.6 m root chunks). Chunks are meshed on the thread pool and
uploaded as `UProceduralMeshComponent`s; an obsolete chunk stays until every chunk replacing its area is ready.
Chunk borders get skirts so LOD seams never show gaps; the outer terrain edge is closed with walls.

Two surface modes (`AVoxelTerrain::bSmoothSurface`, default true):
- Smooth: the voxel steps are evened out. A vertex sits on every voxel corner (0.1 m grid near the camera) at the
  interpolated height; heights come from Catmull-Rom (bicubic) DEM sampling so slopes curve without creases, and
  per-vertex normals (central differences) make the shading continuous. Cells are split along the flatter diagonal.
- Blocky: classic voxel columns (top faces merged along X, walls where a neighbour is lower), heights quantised.

The river channel is the ground clamped between two curves that are continuous in distance-to-centreline
(parabolic bed + 1:3 valley sides as the upper bound, a bank crest just above the water as the lower bound), so
bed, shoreline and banks blend without steps. The water level along the centreline (flowing from the higher end)
is built in three passes in `FVoxelHeightField::BuildRiver`: running minimum of (lowest ground across the channel
- 0.6 m), then a backward pass that limits the slope to 1.5% by lowering upstream levels, then a +-20 m moving
average. Each pass keeps the level non-increasing, so the river only flows downhill; the gentle slope keeps the
surface flat across the channel (each point takes the level of its nearest centreline sample). Last measured
(2026-10-19): 0 uphill steps over 1119 m, 7.4 m total drop, max 1.1 cm drop per 0.5 m, max 0.7 cm tilt across
the channel, banks at least 18 cm above the water.

Data flow:

```
Tools/fetch_heightmap.py --(AWS Terrain Tiles, public)--> Content/Terrain/heightmap.r16 + heightmap.json
Tools/make_ground_texture.py --> Tools/generated/T_Ground.png
Tools/setup_content.py (editor Python) --> /Game/Textures/T_Ground, /Game/Materials/M_VoxelGround, M_Water,
                                           /Game/Maps/TerrainMap (sun, sky, fog, VoxelTerrain, PlayerStart)
Runtime: heightmap files --> FVoxelHeightField (+ river carving) --> FVoxelChunkMesher (worker threads)
         --> AVoxelTerrain (quadtree streaming, ProceduralMeshComponents)
```

## Module / package dependency diagram

```mermaid
graph TD
    subgraph UnrealTest module
        GM[UnrealTestGameMode] --> Pawn[TerrainCameraPawn]
        GM --> VT[VoxelTerrain]
        Pawn --> VT
        VT --> Mesher[VoxelChunkMesher]
        VT --> Field[VoxelHeightField]
        Mesher --> Field
        VT --> Log[UnrealTest.h log category]
    end
    VT --> PMC[(ProceduralMeshComponent plugin)]
    Mesher --> PMC
    Field --> Json[(Json)]
    Pawn --> Engine[(Engine / InputCore)]
    GM --> Engine
    VT --> Engine

    subgraph Tools - Python stdlib only
        Fetch[fetch_heightmap.py]
        Tex[make_ground_texture.py]
        Setup[setup_content.py] --> UnrealPy[(unreal editor API: PythonScriptPlugin, EditorScriptingUtilities)]
        PS[setup.ps1] --> Fetch
        PS --> Tex
        PS --> Setup
    end
    Fetch -. heightmap.r16/json .-> Field
    Setup -. materials, map .-> VT
```

## Files

| Path | Purpose |
|---|---|
| `Source/UnrealTest/VoxelHeightField.*` | Loads the R16 heightmap, bicubic height sampling, micro-relief noise, river rasterisation (distance + monotonic water level) and channel shaping, surface classification. Read-only after setup, so thread-safe. |
| `Source/UnrealTest/VoxelChunkMesher.*` | Builds one chunk's mesh in smooth (corner grid) or blocky (columns + walls) mode, plus skirts, terrain-edge walls and the water surface. |
| `Source/UnrealTest/VoxelTerrain.*` | `AVoxelTerrain` actor: quadtree LOD selection, async builds, component lifetime. Actor origin = north-west corner at lowest elevation (X = east, Y = south). River control points are editable properties. |
| `Source/UnrealTest/TerrainCameraPawn.*` | `ADefaultPawn` subclass: standard controls + speed/boost, on-screen status, verification hooks. |
| `Source/UnrealTest/UnrealTestGameMode.*` | Default game mode; spawns a terrain if the level has none. |
| `Config/DefaultInput.ini` | Legacy input classes so `ADefaultPawn` bindings work. |
| `Config/DefaultEngine.ini`, `DefaultEditor.ini` | Default map/game mode, rendering, analytics/crash upload disabled. |
| `Tools/*` | Content generation scripts (see Data flow). |

Notes:
- `UnrealTest.Build.cs` uses `PCHUsage = NoPCHs` and defines `PLATFORM_HAS_ASAN_INCLUDE=0` to work around a
  UE 5.4 + MSVC 14.44 header error (`__has_feature`, C4668) in `ConcurrentLinearAllocator.h`.
- Triangle winding: Unreal shows the face whose normal is opposite to `(V1-V0) x (V2-V0)`; `FVoxelMeshBuffers::AddQuad`
  and `AddTriangleUp` handle it.
- Mesh UVs must stay small: ProceduralMeshComponent stores them at reduced precision, so absolute world UVs snap
  to 0.5 steps (one texel per metre, looks like a checkerboard). UVs are chunk-local plus the fractional chunk-origin
  offset (`UVOrigin` in `VoxelChunkMesher.cpp`); the ground texture repeats every 2 m.
- `M_VoxelGround` = detail sample (mesh UVs, 2 m) x macro sample (world XY / 1540 cm, ~15 m) x world-space Noise
  (0.78..1.12) x vertex colour x 1.8. Anything that must be continuous across chunks at non-integer tiling has to
  use world position, not mesh UVs (chunk UV offsets only match modulo 1).
- `Tools/setup_content.py` must run with `-AllowCommandletRendering` so materials are translated before saving;
  it fails if `M_VoxelGround` reports no used textures.
- Uncooked runs compile material shaders on demand and show the default WorldGrid material until done;
  `-AutoScreenshot` waits for the shader compiler to be idle.

# Naming conventions

- C++ follows the Unreal coding standard: `A` actors (`AVoxelTerrain`), `U` UObjects/components, `F` plain
  structs/classes (`FVoxelHeightField`), `E` enums (`EVoxelSurface`), `b` prefix for bools, PascalCase for
  types, functions, members and locals; one class per `.h/.cpp` pair named after the class without prefix.
- Voxel-related types start with `Voxel`; camera/game-flow types use `Terrain`/`UnrealTest`.
- Units: terrain-local values are metres (`...Meters`, heights in metres above the lowest point); anything in
  world space is centimetres (Unreal units). Blocky-mode column heights are integers in base-voxel units (0.1 m).
- Assets: `T_` textures, `M_` materials, maps in `/Game/Maps`; non-asset data in `Content/Terrain`.
- Python tools: snake_case files and functions, standard library only (no extra packages).