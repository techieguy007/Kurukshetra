"""Run inside Unreal Editor via Prepare-Project.ps1; not with system Python.

Creates only project-owned placeholder materials and a minimal launch map.
Existing assets are preserved, so this can be rerun after installing the engine.
"""
import unreal


COLORS = {
    "Sand": (0.48, 0.32, 0.15),
    "Stone": (0.17, 0.12, 0.075),
    "Gold": (0.95, 0.50, 0.075),
    "Red": (0.47, 0.045, 0.035),
    "Blue": (0.035, 0.19, 0.40),
    "White": (0.81, 0.80, 0.69),
    "Sky": (0.33, 0.54, 0.65),
}


def create_material(name, color):
    path = "/Game/Prototype/M_" + name
    if unreal.EditorAssetLibrary.does_asset_exist(path):
        unreal.log("Preserving existing asset: " + path)
        return
    material = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        "M_" + name, "/Game/Prototype", unreal.Material, unreal.MaterialFactoryNew()
    )
    if not material:
        raise RuntimeError("Could not create " + path)
    library = unreal.MaterialEditingLibrary
    node = library.create_material_expression(material, unreal.MaterialExpressionConstant3Vector)
    node.set_editor_property("constant", unreal.LinearColor(*color, 1.0))
    if name == "Sky":
        material.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)
        material.set_editor_property("two_sided", True)
        library.connect_material_property(node, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    else:
        library.connect_material_property(node, "", unreal.MaterialProperty.MP_BASE_COLOR)
        roughness = library.create_material_expression(material, unreal.MaterialExpressionConstant)
        roughness.set_editor_property("r", 0.85)
        library.connect_material_property(roughness, "", unreal.MaterialProperty.MP_ROUGHNESS)
        ambient = library.create_material_expression(material, unreal.MaterialExpressionConstant3Vector)
        ambient.set_editor_property("constant", unreal.LinearColor(*(value * 0.18 for value in color), 1.0))
        library.connect_material_property(ambient, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    library.recompile_material(material)
    if not unreal.EditorAssetLibrary.save_loaded_asset(material):
        raise RuntimeError("Could not save " + path)


def main():
    unreal.EditorAssetLibrary.make_directory("/Game/Prototype")
    unreal.EditorAssetLibrary.make_directory("/Game/Maps")
    for name, color in COLORS.items():
        create_material(name, color)
    map_path = "/Game/Maps/Chapter01"
    if not unreal.EditorAssetLibrary.does_asset_exist(map_path):
        levels = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
        if not levels.new_level(map_path):
            raise RuntimeError("Could not create Chapter01")
        actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
        start = actors.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(-2200, 0, 105))
        start.set_actor_label("Chapter start")
        if not levels.save_current_level():
            raise RuntimeError("Could not save Chapter01")
    unreal.log("KURU_BOOTSTRAP_COMPLETE")


main()
