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
