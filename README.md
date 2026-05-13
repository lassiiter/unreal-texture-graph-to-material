# Texture Graph Material Bridge

Texture Graph Material Bridge is an Unreal Engine plugin that helps material graphs consume exported Texture Graph outputs without manually re-wiring texture assets after each export.

The plugin adds material expression nodes for Texture Graph outputs, refreshes linked materials when a Texture Graph is saved, and provides a content browser action to create a linked material from a Texture Graph or Texture Graph Instance.

## What It Does

- Resolves exported Texture Graph outputs as material texture inputs.
- Adds Texture Graph material nodes for texture object and texture sample workflows.
- Re-exports referenced Texture Graph assets on save and recompiles linked materials.
- Creates a basic linked material beside a Texture Graph source.
- Auto-wires common outputs such as base color, normal, emissive, ambient occlusion, roughness, and metallic.
- Supports common ORM packing conventions for ambient occlusion, roughness, and metallic channels.

## Designer-Style Texture Graph Nodes

The plugin adds UE-native, Designer-style Texture Graph nodes for common material-authoring workflows, complementing what Unreal already provides out-of-the-box.

### Native UE Equivalents

Do not look for the following nodes in this plugin; Unreal Engine's Texture Graph natively provides their equivalents:

| Designer-style node | Native Texture Graph equivalent |
| --- | --- |
| Blend | `Blend` / `Math` nodes |
| Levels | `Levels` |
| Transform | `Transform` |
| Uniform Color | `Color` / `Vector` constants |
| Output | `Output` |
| Histogram Scan / Range | `Histogram` |
| Invert | `Invert` / `OneMinus` |
| Edge Detect | `Edge Detect` |
| Shape | `Shape` |
| Perlin Noise | `Noise` nodes |
| Warp / Directional Warp | `Warp` |
| Blur | `Blur` |
| Normal From Height | `Normal From Height` |

### Plugin Nodes

The plugin adds the following GPU-backed, Designer-style nodes:

| Category | Nodes |
| --- | --- |
| Base Pattern & Scattering | Tile Generator, Tile Sampler, Shape Splatter |
| Flood Fill & Converters | Flood Fill, Flood Fill to Random Grayscale, Flood Fill to Random Color, Flood Fill to Gradient, Flood Fill to Position, Flood Fill to BBox Size, Flood Fill Mapper |
| Noise & Grunge | Cells 1, Cells 2, Cells 3, Cells 4, Clouds 2, BnW Spots, Grunge Dirt |
| Warping & Distance | Multi Directional Warp, Non-Uniform Directional Warp, Directional Distance |
| Filters & Blur | Blur HQ, Slope Blur, Gradient Map, Highpass, Luminance Highpass |
| Normal & Surface Details | Bevel, Ambient Occlusion HBAO, Curvature Smooth, Curvature Sobel, Normal Combine |
| Blending | Multi-Material Blend |

## Creating a Texture Graph Node

Start node work with the local agent skill at `skills/add-tgmb-node/SKILL.md`. The skill is the checklist an agent should follow when turning a natural-language node request into a complete Texture Graph expression, transform, and shader implementation.

### Agentic Skill Flow

1. Convert the request into a node spec: display name, C++ suffix, category, tooltip, input pins, output pins, settings, defaults, clamp ranges, shader behavior, fallback behavior, and tiling expectations.
2. Read `skills/add-tgmb-node/references/project-patterns.md`, then inspect the closest existing node before editing.
3. Choose the nearest local pattern:
   - Texture filter: source texture plus one output, like Highpass, Bevel, or Slope Blur.
   - Generator: settings-only output, like Clouds 2, Cells 1, or Tile Generator.
   - Multi-input blend: several source textures, like Normal Combine or Multi-Material Blend.
   - Multi-output node: one expression evaluates multiple transform outputs, like Flood Fill or Curvature Smooth.
4. Edit the shared designer-node files unless the request explicitly calls for a new layout.
5. Keep the expression, transform declaration, transform implementation, shader wrapper, `IMPLEMENT_GLOBAL_SHADER`, and HLSL entry point wired together with matching names.
6. Validate by searching for the new suffix across `Source` and `Shaders`, then run the Unreal build script when the local Unreal paths are available.

### Implementation Details

Most Designer-style nodes in this plugin follow a small, repeatable path: add a `UTG_Expression_TGMB_*` class, pass its pins and settings into a transform, bind that transform to a global shader, then implement the shader entry point.

When adding a node, start by choosing the closest existing pattern:

- **Texture filters** read one or more `FTG_Texture` inputs and write one output. Examples: Highpass, Bevel, Slope Blur.
- **Generators** create an output from settings alone. Examples: Clouds 2, Cells 1, Tile Generator.
- **Multi-input adjustments** combine several texture inputs. Examples: Normal Combine, Multi-Material Blend.
- **Multi-output/data nodes** produce an intermediate data texture plus user-facing outputs. Examples: Flood Fill and Curvature Smooth.

Keep the main pieces synchronized:

1. Add the expression class, pins, settings, title, and tooltip in `Source/TextureGraphMaterialBridge/Public/TextureGraphMaterialBridgeDesignerExpressions.h`.
2. Implement `Evaluate` in `Source/TextureGraphMaterialBridge/Private/TextureGraphMaterialBridgeDesignerExpressions.cpp`. The expression should call a named `UE::TextureGraphMaterialBridge::FDesignerTransforms::Create*` helper and pass `InContext->Cycle`, the output descriptor, `InContext->TargetId`, input `RasterBlob`s, and settings.
3. Declare the shader wrapper class and `FDesignerTransforms::Create*` function in `Source/TextureGraphMaterialBridge/Private/TextureGraphMaterialBridgeDesignerTransforms.h`.
4. Add the `IMPLEMENT_GLOBAL_SHADER` entry and transform implementation in `Source/TextureGraphMaterialBridge/Private/TextureGraphMaterialBridgeDesignerTransforms.cpp`. Use local helpers such as `CombineIfNeeded`, `BuildOutputDesc`, `CreateShaderJob`, `TextureHelper::GetBlack()`, and `JobArg_ForceTiling` where they match the surrounding node pattern.
5. Add the HLSL entry point in `Shaders/Expressions/TGMB_DesignerNodes.usf`, and put shared reusable shader helpers in `Shaders/TGMB_DesignerCommon.ush` only when multiple shaders benefit.

The C++ shader parameter names in `AddArg(...)` calls must match the HLSL globals exactly. After wiring a node, search for the suffix across `Source` and `Shaders` to confirm the expression, transform declaration, transform implementation, `IMPLEMENT_GLOBAL_SHADER`, and shader function all exist. Then run the build script when the local Unreal paths are available:

```powershell
powershell -ExecutionPolicy Bypass -File "scripts\Build-TextureGraphMaterialBridgeInProject.ps1"
```

### How Deep Can a Node Go?

A simple node can be only a Texture Graph expression plus a single pixel shader job. The system can also go much deeper when the node needs custom GPU orchestration.

Flood Fill is the current deep example. The `UTG_Expression_TGMB_FloodFill` node exposes normal user settings such as threshold, connectivity, output mode, seed, and gradient angle, but its `Evaluate` function first creates a hidden `FloodFillData` texture and then renders the selected visible output from that data. Under the hood, `CreateFloodFillData` uses a custom compute `FxMaterial` instead of the usual one-pass pixel shader path. It allocates structured buffers for labels and bounds, dispatches several compute kernels from `Shaders/Expressions/TGMB_FloodFillCompute.usf`, and writes a float RGBA data texture where each active pixel stores its connected region bounds.

That data texture then becomes a reusable contract for cheaper converter nodes. `Flood Fill to Random Grayscale`, `Random Color`, `Gradient`, `Position`, `BBox Size`, and `Mapper` all read the same encoded bounds through pixel shaders in `TGMB_DesignerNodes.usf`. This means a node can scale from a lightweight shader wrapper all the way to a multi-pass compute pipeline with intermediate buffers, custom result descriptors, non-tiled execution, and companion nodes that consume its data.

### Out of Scope

The following items are intentionally out of scope for this pass:
- Exact Substance Designer pixel parity (nodes provide useful UE-native approximations).
- Bundled bitmap grunge libraries (grunge/dirt nodes are procedural shader-generated textures).
- Material asset inputs.
- Mesh-bake generators.

## Typical Workflow

1. Configure Texture Graph outputs with valid export settings.
2. Export or save the Texture Graph so output texture assets exist.
3. Use the Texture Graph material nodes inside a material, or right-click a Texture Graph asset and choose **Create Linked Material**.
4. Continue editing the Texture Graph; linked materials refresh when the graph is saved.

## Requirements

- Unreal Engine 5.7.
- The built-in Texture Graph plugin enabled.
- Texture Graph outputs must be configured to export Unreal texture assets.

## Current Status

This is an early plugin prototype. It focuses on the core bridge between Texture Graph exports and material graphs, with a small editor workflow for linked material creation. Documentation, validation coverage, packaging polish, and broader material mapping options are natural next steps.
