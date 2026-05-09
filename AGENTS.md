# Agent Guide

This file applies to the whole repository.

## Project Overview

Texture Graph Material Bridge is an Unreal Engine 5.7 plugin. It bridges Texture Graph export outputs into material graphs, adds material expression nodes for Texture Graph outputs, refreshes linked materials on save, and includes UE-native, Designer-style Texture Graph nodes backed by plugin shaders.

## Repository Layout

- `TextureGraphMaterialBridge.uplugin`: plugin descriptor. Keep module names, loading phases, engine version, and Texture Graph plugin dependency in sync with code changes.
- `Source/TextureGraphMaterialBridge`: runtime module. Keep this free of editor-only dependencies.
- `Source/TextureGraphMaterialBridgeEditor`: editor module. Asset tools, content browser actions, graph/editor UI, material creation, and save hooks live here.
- `Shaders`: plugin shader sources. Shader paths are referenced as `/Plugin/TextureGraphMaterialBridge/...`.
- `scripts`: local development helpers. The build script copies the plugin into a local UE project before invoking Unreal Build Tool.

## Coding Conventions

- Follow Unreal Engine C++ style: `F`/`U`/`E`/`T` prefixes, `TEXT(...)`, `TArray`, `TMap`, `FName`, `FString`, `check`, `ensure`, and UE logging categories where appropriate.
- Match the surrounding formatting in each file. Existing C++ generally uses tabs for indentation and Unreal brace style.
- Put runtime-safe code in the runtime module and editor-only code in the editor module. If a new include requires another module, update the matching `.Build.cs` file.
- Prefer small, named helpers inside the relevant namespace over broad abstractions.
- Keep public headers minimal. Do not expose private implementation details unless another module genuinely needs them.

## Texture Graph Node Changes

When adding or changing Designer-style Texture Graph nodes, keep the related pieces synchronized:

- Expression declarations and user-facing properties in `Source/TextureGraphMaterialBridge/Public/TextureGraphMaterialBridgeDesignerExpressions.h`.
- Expression implementation and transform calls in `Source/TextureGraphMaterialBridge/Private/TextureGraphMaterialBridgeDesignerExpressions.cpp`.
- Transform declarations in `Source/TextureGraphMaterialBridge/Private/TextureGraphMaterialBridgeDesignerTransforms.h`.
- Transform implementations and `IMPLEMENT_GLOBAL_SHADER` entries in `Source/TextureGraphMaterialBridge/Private/TextureGraphMaterialBridgeDesignerTransforms.cpp`.
- Shader functions and parameter names in `Shaders/Expressions/TGMB_DesignerNodes.usf`.

Shader parameter names in C++ `AddArg(...)` calls must match the corresponding `.usf` globals exactly.

## Build And Verification

Preferred local build command from the repository root:

```powershell
powershell -ExecutionPolicy Bypass -File "scripts\Build-TextureGraphMaterialBridgeInProject.ps1"
```

That script expects the local UE 5.7 install/project paths defined inside it. If those paths are unavailable, do not guess silently; report that the build could not be run and why.

For shader or Texture Graph node changes, a successful C++ build is not always enough. Verify in Unreal when possible by loading the plugin, compiling shaders, creating or editing a Texture Graph, and checking that affected nodes render and save without errors.

For material bridge changes, verify the main workflow when possible:

1. Configure Texture Graph outputs with export settings.
2. Export or save the graph so texture assets exist.
3. Create or update a linked material.
4. Save the graph again and confirm linked material nodes refresh.

## Git And Generated Files

- Preserve user work. Do not revert modified files unless explicitly asked.
- Avoid committing Unreal-generated artifacts such as `Binaries`, `Intermediate`, `Saved`, `.vs`, derived data, or local project copies.
- Keep changes scoped to the plugin source, shaders, scripts, and docs needed for the task.
- If a file has existing unrelated edits, work around them carefully and call out any conflicts or verification gaps.
