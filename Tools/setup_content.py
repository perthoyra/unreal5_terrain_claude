"""Unreal Editor script: creates the project content.

    /Game/Textures/T_Ground        imported from Tools/generated/T_Ground.png
    /Game/Materials/M_VoxelGround  texture x vertex colour (voxel surface tint)
    /Game/Materials/M_Water        translucent water
    /Game/Maps/TerrainMap          sun, sky, fog, AVoxelTerrain, PlayerStart

Run headless (see Tools/setup.ps1):
    UnrealEditor-Cmd.exe UnrealTest.uproject -run=pythonscript -script=Tools/setup_content.py -AllowCommandletRendering

-AllowCommandletRendering is required: without a renderer the materials are never translated, their cached
texture list stays empty and the game samples the engine's default checker texture instead of T_Ground.
"""

import json
import math
import os
import struct

import unreal

PROJECT_DIR = unreal.Paths.convert_relative_path_to_full(unreal.Paths.project_dir())
TERRAIN_ORIGIN = unreal.Vector(-100000.0, -100000.0, 0.0)  # 2 km terrain centred on the world origin

asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
mel = unreal.MaterialEditingLibrary
eal = unreal.EditorAssetLibrary


def fresh_asset(name, package_path, asset_class, factory):
    path = f"{package_path}/{name}"
    if eal.does_asset_exist(path):
        eal.delete_asset(path)
    return asset_tools.create_asset(name, package_path, asset_class, factory)


def import_ground_texture():
    task = unreal.AssetImportTask()
    task.filename = os.path.join(PROJECT_DIR, "Tools", "generated", "T_Ground.png")
    task.destination_path = "/Game/Textures"
    task.destination_name = "T_Ground"
    task.replace_existing = True
    task.automated = True
    task.save = True
    asset_tools.import_asset_tasks([task])
    texture = unreal.load_asset("/Game/Textures/T_Ground")
    texture.set_editor_property("srgb", True)
    eal.save_asset("/Game/Textures/T_Ground")
    return texture


def create_ground_material(texture):
    """BaseColor = detail texture x macro texture x world noise x vertex colour (voxel surface tint).

    The detail sample repeats every 2 m (mesh UVs). A second, world-projected sample of the same texture
    (~15.4 m, deliberately not a multiple of 2 m) and a low-frequency world-space noise break up the visible repeat.
    """
    mat = fresh_asset("M_VoxelGround", "/Game/Materials", unreal.Material, unreal.MaterialFactoryNew())

    detail = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSample, -1100, -300)
    detail.set_editor_property("texture", texture)

    # Macro UVs from world XY (~15.4 m per repeat): mesh UVs only line up across chunks at integer tiling,
    # so a 0.13x mesh-UV tiling would show seams at chunk borders.
    world_pos = mel.create_material_expression(mat, unreal.MaterialExpressionWorldPosition, -1700, 0)
    world_xy = mel.create_material_expression(mat, unreal.MaterialExpressionComponentMask, -1550, 0)
    world_xy.set_editor_property("r", True)
    world_xy.set_editor_property("g", True)
    macro_uv = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply, -1400, 0)
    macro_uv.set_editor_property("const_b", 1.0 / 1540.0)
    macro = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSample, -1100, 0)
    macro.set_editor_property("texture", texture)
    mel.connect_material_expressions(world_pos, "", world_xy, "")
    mel.connect_material_expressions(world_xy, "", macro_uv, "A")
    mel.connect_material_expressions(macro_uv, "", macro, "UVs")

    # World-space brightness variation (~6 m features); position input defaults to absolute world position.
    noise = mel.create_material_expression(mat, unreal.MaterialExpressionNoise, -1100, 300)
    noise.set_editor_property("scale", 0.0016)
    noise.set_editor_property("levels", 3)
    noise.set_editor_property("output_min", 0.78)
    noise.set_editor_property("output_max", 1.12)

    vcol = mel.create_material_expression(mat, unreal.MaterialExpressionVertexColor, -1100, 600)

    detail_macro = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply, -800, -150)
    with_noise = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply, -600, 0)
    mul = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply, -400, 0)
    boost = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply, -200, 0)
    boost.set_editor_property("const_b", 1.8)  # the macro sample averages ~0.7, compensate for it
    rough = mel.create_material_expression(mat, unreal.MaterialExpressionConstant, -200, 200)
    rough.set_editor_property("r", 0.92)

    mel.connect_material_expressions(detail, "RGB", detail_macro, "A")
    mel.connect_material_expressions(macro, "RGB", detail_macro, "B")
    mel.connect_material_expressions(detail_macro, "", with_noise, "A")
    mel.connect_material_expressions(noise, "", with_noise, "B")
    mel.connect_material_expressions(with_noise, "", mul, "A")
    mel.connect_material_expressions(vcol, "", mul, "B")
    mel.connect_material_expressions(mul, "", boost, "A")
    mel.connect_material_property(boost, "", unreal.MaterialProperty.MP_BASE_COLOR)
    mel.connect_material_property(rough, "", unreal.MaterialProperty.MP_ROUGHNESS)
    mel.recompile_material(mat)
    if not mel.get_used_textures(mat):
        raise RuntimeError("M_VoxelGround was not translated; run with -AllowCommandletRendering")
    eal.save_asset("/Game/Materials/M_VoxelGround")
    return mat


def create_water_material():
    mat = fresh_asset("M_Water", "/Game/Materials", unreal.Material, unreal.MaterialFactoryNew())
    mat.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
    mat.set_editor_property("translucency_lighting_mode", unreal.TranslucencyLightingMode.TLM_SURFACE)

    color = mel.create_material_expression(mat, unreal.MaterialExpressionConstant3Vector, -400, -100)
    color.set_editor_property("constant", unreal.LinearColor(0.02, 0.11, 0.16, 1.0))
    opacity = mel.create_material_expression(mat, unreal.MaterialExpressionConstant, -400, 100)
    opacity.set_editor_property("r", 0.72)
    rough = mel.create_material_expression(mat, unreal.MaterialExpressionConstant, -400, 200)
    rough.set_editor_property("r", 0.05)
    spec = mel.create_material_expression(mat, unreal.MaterialExpressionConstant, -400, 300)
    spec.set_editor_property("r", 0.8)

    mel.connect_material_property(color, "", unreal.MaterialProperty.MP_BASE_COLOR)
    mel.connect_material_property(opacity, "", unreal.MaterialProperty.MP_OPACITY)
    mel.connect_material_property(rough, "", unreal.MaterialProperty.MP_ROUGHNESS)
    mel.connect_material_property(spec, "", unreal.MaterialProperty.MP_SPECULAR)
    mel.recompile_material(mat)
    eal.save_asset("/Game/Materials/M_Water")
    return mat


def terrain_height_m(x_m, y_m):
    """Height (m above terrain minimum) from the heightmap at terrain-local metres."""
    terrain_dir = os.path.join(PROJECT_DIR, "Content", "Terrain")
    with open(os.path.join(terrain_dir, "heightmap.json")) as f:
        meta = json.load(f)
    res = meta["resolution"]
    col = min(max(int(round(x_m / meta["spacing_m"])), 0), res - 1)
    row = min(max(int(round(y_m / meta["spacing_m"])), 0), res - 1)
    with open(os.path.join(terrain_dir, "heightmap.r16"), "rb") as f:
        f.seek((row * res + col) * 2)
        value = struct.unpack("<H", f.read(2))[0]
    return value / 65535.0 * (meta["max_elevation_m"] - meta["min_elevation_m"])


def create_map(ground_mat, water_mat):
    map_path = "/Game/Maps/TerrainMap"
    if eal.does_asset_exist(map_path):
        eal.delete_asset(map_path)
    les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
    les.new_level(map_path)
    actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

    # Sun in the south-west, so the mountain face seen from the PlayerStart is lit.
    sun = actors.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0, 0, 50000), unreal.Rotator(0, -40, -50))
    sun.set_actor_label("Sun")
    sun_comp = sun.get_component_by_class(unreal.DirectionalLightComponent)
    sun_comp.set_editor_property("atmosphere_sun_light", True)
    sun_comp.set_editor_property("intensity", 10.0)
    sun_comp.set_editor_property("mobility", unreal.ComponentMobility.MOVABLE)
    sun_comp.set_editor_property("dynamic_shadow_distance_movable_light", 300000.0)

    actors.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector(0, 0, 0)).set_actor_label("SkyAtmosphere")

    sky = actors.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 60000))
    sky.set_actor_label("SkyLight")
    sky_comp = sky.get_component_by_class(unreal.SkyLightComponent)
    sky_comp.set_editor_property("mobility", unreal.ComponentMobility.MOVABLE)
    sky_comp.set_editor_property("real_time_capture", True)

    fog = actors.spawn_actor_from_class(unreal.ExponentialHeightFog, unreal.Vector(0, 0, 0))
    fog.set_actor_label("HeightFog")
    fog.get_component_by_class(unreal.ExponentialHeightFogComponent).set_editor_property("fog_density", 0.002)

    terrain_class = unreal.load_class(None, "/Script/UnrealTest.VoxelTerrain")
    terrain = actors.spawn_actor_from_class(terrain_class, TERRAIN_ORIGIN)
    terrain.set_actor_label("VoxelTerrain")
    terrain.set_editor_property("terrain_material", ground_mat)
    terrain.set_editor_property("water_material", water_mat)

    # Start south-west of the mountain, 40 m above the ground, looking at the summit.
    start_x, start_y = 450.0, 1500.0
    ground = terrain_height_m(start_x, start_y)
    start = unreal.Vector(TERRAIN_ORIGIN.x + start_x * 100.0, TERRAIN_ORIGIN.y + start_y * 100.0, (ground + 40.0) * 100.0)
    yaw = math.degrees(math.atan2(1000.0 - start_y, 1000.0 - start_x))
    player_start = actors.spawn_actor_from_class(unreal.PlayerStart, start, unreal.Rotator(0, 0, yaw))
    player_start.set_actor_label("PlayerStart")

    les.save_current_level()


def main():
    texture = import_ground_texture()
    ground_mat = create_ground_material(texture)
    water_mat = create_water_material()
    create_map(ground_mat, water_mat)
    unreal.log("UnrealTest content setup complete")


main()
