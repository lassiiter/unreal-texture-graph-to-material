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

- **Blend** (Native: `Blend` / `Math` nodes)
- **Levels** (Native: `Levels`)
- **Transform** (Native: `Transform`)
- **Uniform Color** (Native: `Color` / `Vector` constants)
- **Output** (Native: `Output`)
- **Histogram Scan / Range** (Native: `Histogram`)
- **Invert** (Native: `Invert` / `OneMinus`)
- **Edge Detect** (Native: `Edge Detect`)
- **Shape** (Native: `Shape`)
- **Perlin Noise** (Native: `Noise` nodes)
- **Warp / Directional Warp** (Native: `Warp`)
- **Blur** (Native: `Blur`)
- **Normal From Height** (Native: `Normal From Height`)

### Plugin Nodes

The plugin adds the following GPU-backed, Designer-style nodes:

#### Base Pattern & Scattering
- Tile Generator
- Tile Sampler
- Shape Splatter

#### Flood Fill & Converters
- Flood Fill
- Flood Fill to Random Grayscale
- Flood Fill to Random Color
- Flood Fill to Gradient
- Flood Fill to Position
- Flood Fill Mapper

#### Noise & Grunge
- Cells 1
- Cells 2
- Cells 3
- Cells 4
- Clouds 2
- BnW Spots
- Grunge Dirt

#### Warping & Distance
- Multi Directional Warp
- Non-Uniform Directional Warp
- Directional Distance

#### Filters & Blur
- Blur HQ
- Slope Blur
- Gradient Map
- Highpass
- Luminance Highpass

#### Normal & Surface Details
- Bevel
- Ambient Occlusion HBAO
- Curvature Smooth
- Curvature Sobel
- Normal Combine

#### Blending
- Multi-Material Blend

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
