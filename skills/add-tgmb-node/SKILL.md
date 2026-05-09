---
name: add-tgmb-node
description: Add a custom designer-style Texture Graph node to the TextureGraphMaterialBridge Unreal plugin from a natural-language description. Use when Codex is asked to create, scaffold, implement, or wire a new `UTG_Expression_TGMB_*` node, shader-backed Texture Graph expression, procedural/filter/adjustment node, node settings, pins, or shader entry point in this repo.
---

# Add TGMB Node

## Workflow

Use this skill inside the `unreal-texture-graph-to-shader` repository. Start by reading `references/project-patterns.md`, then inspect the closest existing node before editing.

1. Turn the request into a node spec:
   - Display name, C++ suffix, category, tooltip.
   - Input texture pins, output texture pins, settings, defaults, and UI clamp ranges.
   - Shader behavior, expected fallback behavior for missing inputs, and whether the node is tile-aware.

2. Choose the closest local pattern:
   - Texture filter: source texture plus one output, like `Highpass`, `Bevel`, or `SlopeBlur`.
   - Generator: settings-only output, like `Clouds2`, `Cells1`, or `TileGenerator`.
   - Multi-input blend: several source textures, like `NormalCombine` or `MultiMaterialBlend`.
   - Multi-output node: one expression evaluates multiple transform outputs, like `FloodFill` or `CurvatureSmooth`.

3. Edit the shared designer-node files unless the user explicitly asks for a new file layout:
   - `Source/TextureGraphMaterialBridge/Public/TextureGraphMaterialBridgeDesignerExpressions.h`
   - `Source/TextureGraphMaterialBridge/Private/TextureGraphMaterialBridgeDesignerExpressions.cpp`
   - `Source/TextureGraphMaterialBridge/Private/TextureGraphMaterialBridgeDesignerTransforms.h`
   - `Source/TextureGraphMaterialBridge/Private/TextureGraphMaterialBridgeDesignerTransforms.cpp`
   - `Shaders/Expressions/TGMB_DesignerNodes.usf`
   - `Shaders/TGMB_DesignerCommon.ush` only for reusable shader helpers.

4. Keep the wiring complete:
   - Add the `UTG_Expression_TGMB_*` class with `TG_DECLARE_EXPRESSION`, `Evaluate`, title, tooltip, `FTG_Texture` pins, and `TG_Setting` metadata.
   - Add the `Evaluate` implementation that calls `UE::TextureGraphMaterialBridge::FDesignerTransforms::Create*`.
   - Add the `FSH_TGMB*` shader wrapper class and parameter struct.
   - Add the `FDesignerTransforms::Create*` declaration and implementation.
   - Add `IMPLEMENT_GLOBAL_SHADER`.
   - Add the `FSH_TGMB*` pixel shader function and any needed global parameters.

5. Preserve project conventions:
   - Use `MinimalAPI` for expression classes.
   - Use `FTG_Texture` for Texture Graph image pins.
   - Use `TextureHelper::GetBlack()` for absent color/source data unless a mask/scale/default strength should be white.
   - Use `CombineIfNeeded` before passing input blobs to shader jobs.
   - Use `BuildOutputDesc(DesiredDesc, Reference)` for input-sized outputs and `BuildOutputDesc(DesiredDesc, nullptr)` for generators.
   - Use `FTileInfo` and `TileInfo_fromCurrentTileToLayer(UV)` for tile-aware/procedural UVs.
   - Add `JobArg_ForceTiling` for procedural generators that should evaluate per tile like existing cells/noise nodes.

6. Validate after editing:
   - Run `rg -n "NewNodeSuffix|FSH_TGMBNewNode|CreateNewNode" Source Shaders` to confirm every wiring point exists.
   - Build with the project script when the Unreal environment is available: `powershell -ExecutionPolicy Bypass -File scripts\Build-TextureGraphMaterialBridgeInProject.ps1`.
   - If building is unavailable, state that clearly and list the structural checks performed.

## Output Style

When finished, summarize the node behavior, files changed, and verification. Mention any assumptions made from an underspecified description.
