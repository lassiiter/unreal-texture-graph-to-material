import os

def inject_in_file(path, search_str, inject_str, append=False):
    with open(path, 'r', encoding='utf-8') as f:
        content = f.read()
    if append:
        content = content + "\n" + inject_str
    else:
        if search_str not in content:
            print(f"Warning: search string not found in {path}")
        content = content.replace(search_str, inject_str + "\n" + search_str)
    with open(path, 'w', encoding='utf-8') as f:
        f.write(content)

expressions_h = r"Source\TextureGraphMaterialBridge\Public\TextureGraphMaterialBridgeDesignerExpressions.h"
expressions_cpp = r"Source\TextureGraphMaterialBridge\Private\TextureGraphMaterialBridgeDesignerExpressions.cpp"
transforms_h = r"Source\TextureGraphMaterialBridge\Private\TextureGraphMaterialBridgeDesignerTransforms.h"
transforms_cpp = r"Source\TextureGraphMaterialBridge\Private\TextureGraphMaterialBridgeDesignerTransforms.cpp"

nodes = [
    {"name": "FloodFillToRandomGrayscale", "cat": "Filter", "desc": "Converts flood fill data to random grayscale.", "inputs": ["Flood Fill Data:Source"], "settings": ["int32 Seed=0"]},
    {"name": "FloodFillToRandomColor", "cat": "Filter", "desc": "Converts flood fill data to random color.", "inputs": ["Flood Fill Data:Source"], "settings": ["int32 Seed=0"]},
    {"name": "FloodFillToGradient", "cat": "Filter", "desc": "Converts flood fill data to gradient.", "inputs": ["Flood Fill Data:Source"], "settings": ["float GradientAngle=0.0f", "float RotationJitter=0.0f", "int32 Seed=0"]},
    {"name": "FloodFillToPosition", "cat": "Filter", "desc": "Converts flood fill data to position.", "inputs": ["Flood Fill Data:Source"], "settings": []},
    {"name": "FloodFillMapper", "cat": "Filter", "desc": "Maps a texture into flood fill cells.", "inputs": ["Flood Fill Data:Source", "Map:MaskTexture"], "settings": []},
    {"name": "MultiDirectionalWarp", "cat": "Filter", "desc": "Warps input in multiple directions based on mask intensity.", "inputs": ["Source", "Intensity Map:MaskTexture"], "settings": ["float Intensity=1.0f", "float Rotation=0.5f", "float GradientAngle=0.0f"]},
    {"name": "NonUniformDirectionalWarp", "cat": "Filter", "desc": "Warps input directionally masked by intensity.", "inputs": ["Source", "Intensity Map:MaskTexture"], "settings": ["float Intensity=1.0f", "float GradientAngle=0.0f"]},
    {"name": "DirectionalDistance", "cat": "Filter", "desc": "Computes distance to nearest mask pixel directionally.", "inputs": ["Source"], "settings": ["float Distance=0.5f", "float GradientAngle=0.0f", "int32 Samples=32", "float Threshold=0.5f"]},
    {"name": "ShapeSplatter", "cat": "Procedural", "desc": "Splatter shapes along a grid masked by a background map.", "inputs": ["Pattern:Source", "Background:MaskTexture"], "settings": ["float CountX=8", "float CountY=8", "float Scale=1.0f", "float Threshold=0.1f"]},
    {"name": "Clouds2", "cat": "Procedural", "desc": "Generates cloud-like noise.", "inputs": [], "settings": ["float Scale=4.0f", "float Contrast=1.0f", "float Bias=0.0f", "float OffsetX=0.0f", "float OffsetY=0.0f", "int32 Seed=0"]},
    {"name": "BnWSpots", "cat": "Procedural", "desc": "Generates black and white spots.", "inputs": [], "settings": ["float Scale=4.0f", "float Threshold=0.5f", "float Smoothness=0.1f", "float OffsetX=0.0f", "float OffsetY=0.0f", "int32 Seed=0"]},
    {"name": "GrungeDirt", "cat": "Procedural", "desc": "Generates grunge dirt noise.", "inputs": [], "settings": ["float Scale=4.0f", "float Contrast=1.0f", "float Bias=0.0f", "float OffsetX=0.0f", "float OffsetY=0.0f", "int32 Seed=0"]},
    {"name": "Highpass", "cat": "Filter", "desc": "Applies a highpass filter.", "inputs": ["Source"], "settings": ["float Radius=4.0f", "float Contrast=1.0f"]},
    {"name": "LuminanceHighpass", "cat": "Filter", "desc": "Applies a luminance highpass filter.", "inputs": ["Source"], "settings": ["float Radius=4.0f", "float Contrast=1.0f"]},
    {"name": "CurvatureSobel", "cat": "Filter", "desc": "Computes curvature using Sobel edge detection.", "inputs": ["Source"], "settings": ["float Radius=1.0f", "float Intensity=1.0f"]}
]

expr_h = ""
for n in nodes:
    expr_h += f'UCLASS(MinimalAPI)\nclass UTG_Expression_TGMB_{n["name"]} : public UTG_Expression {{\n\tGENERATED_BODY()\npublic:\n\tTG_DECLARE_EXPRESSION(TG_Category::{n["cat"]});\n\tvirtual void Evaluate(FTG_EvaluationContext* InContext) override;\n\tvirtual FTG_Name GetDefaultName() const override {{ return TEXT("{n["name"].replace("To", " to ").replace("Fill", " Fill ").replace("Multi", "Multi ").replace("NonUniform", "Non-Uniform ").replace("Directional", "Directional ").replace("Shape", "Shape ").replace("Clouds", "Clouds ").replace("BnW", "BnW ").replace("Grunge", "Grunge ").replace("Luminance", "Luminance ").replace("Curvature", "Curvature ").strip()}"); }}\n\tvirtual FText GetTooltipText() const override {{ return FText::FromString(TEXT("{n["desc"]}")); }}\n'
    for inp in n["inputs"]:
        parts = inp.split(":")
        disp = parts[0]
        var = parts[1] if len(parts) > 1 else "Source"
        expr_h += f'\tUPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "{disp}")) FTG_Texture {var};\n'
    for s in n["settings"]:
        t, v = s.split(" ")[0], s.split(" ")[1]
        var, val = v.split("=")
        expr_h += f'\tUPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) {t} {var} = {val};\n'
    expr_h += '\tUPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;\n};\n\n'

trans_h_classes = ""
trans_h_methods = ""
for n in nodes:
    trans_h_classes += f'class FSH_TGMB{n["name"]} : public FSH_Base {{\npublic:\n\tDECLARE_EXPORTED_GLOBAL_SHADER(FSH_TGMB{n["name"]}, UE_API);\n\tSHADER_USE_PARAMETER_STRUCT(FSH_TGMB{n["name"]}, FSH_Base);\n\tBEGIN_SHADER_PARAMETER_STRUCT(FParameters, )\n\t\tSHADER_PARAMETER_STRUCT(FTileInfo, TileInfo)\n\t\tSHADER_PARAMETER_STRUCT(FStandardSamplerStates, SamplerStates)\n'
    for inp in n["inputs"]:
        parts = inp.split(":")
        var = parts[1] if len(parts) > 1 else "Source"
        var_shader = "SourceTexture" if var == "Source" else "MaskTexture"
        trans_h_classes += f'\t\tSHADER_PARAMETER_TEXTURE(Texture2D, {var_shader})\n'
    for s in n["settings"]:
        t, v = s.split(" ")[0], s.split(" ")[1].split("=")[0]
        trans_h_classes += f'\t\tSHADER_PARAMETER({t}, {v})\n'
    trans_h_classes += '\tEND_SHADER_PARAMETER_STRUCT()\n\tTEXTURE_ENGINE_DEFAULT_PERMUTATION;\n\tTEXTUREGRAPH_ENGINE_DEFAULT_COMPILATION_ENV;\n};\n\n'
    
    args = ["MixUpdateCyclePtr Cycle", "BufferDescriptor DesiredDesc", "int32 TargetId"]
    for inp in n["inputs"]:
        parts = inp.split(":")
        var = parts[1] if len(parts) > 1 else "Source"
        args.append(f"TiledBlobPtr {var}")
    for s in n["settings"]:
        args.append(s.split("=")[0])
    trans_h_methods += f'\t\tstatic TiledBlobPtr Create{n["name"]}({", ".join(args)});\n'

trans_cpp_impls = ""
for n in nodes:
    trans_cpp_impls += f'IMPLEMENT_GLOBAL_SHADER(FSH_TGMB{n["name"]}, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMB{n["name"]}", SF_Pixel);\n'

trans_cpp_methods = ""
for n in nodes:
    args = ["MixUpdateCyclePtr Cycle", "BufferDescriptor DesiredDesc", "int32 TargetId"]
    for inp in n["inputs"]:
        parts = inp.split(":")
        var = parts[1] if len(parts) > 1 else "Source"
        args.append(f"TiledBlobPtr {var}")
    for s in n["settings"]:
        args.append(s.split("=")[0])
    trans_cpp_methods += f'\n\tTiledBlobPtr FDesignerTransforms::Create{n["name"]}({", ".join(args)}) {{\n'
    
    primary_input = None
    for inp in n["inputs"]:
        parts = inp.split(":")
        var = parts[1] if len(parts) > 1 else "Source"
        if primary_input is None: primary_input = var
        trans_cpp_methods += f'\t\tTiledBlobPtr Combined{var} = CombineIfNeeded(Cycle, TargetId, {var} ? {var} : TextureHelper::GetBlack());\n'
    
    trans_cpp_methods += f'\t\tBufferDescriptor Desc = BuildOutputDesc(DesiredDesc, {"Combined" + primary_input if primary_input else "nullptr"});\n'
    trans_cpp_methods += f'\t\tFTileInfo TileInfo;\n'
    trans_cpp_methods += f'\t\tJobUPtr RenderJob = CreateShaderJob<FSH_TGMB{n["name"]}>(Cycle, TargetId, TEXT("TGMB_{n["name"]}"));\n'
    trans_cpp_methods += f'\t\tRenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))\n'
    for inp in n["inputs"]:
        parts = inp.split(":")
        var = parts[1] if len(parts) > 1 else "Source"
        var_shader = "SourceTexture" if var == "Source" else "MaskTexture"
        trans_cpp_methods += f'\t\t\t->AddArg(ARG_BLOB(Combined{var}, "{var_shader}"))\n'
    for s in n["settings"]:
        t, v = s.split(" ")[0], s.split(" ")[1].split("=")[0]
        if t == "int32":
            trans_cpp_methods += f'\t\t\t->AddArg(ARG_INT({v}, "{v}"))\n'
        else:
            trans_cpp_methods += f'\t\t\t->AddArg(ARG_FLOAT({v}, "{v}"))\n'
    
    # Needs to handle ForceTiling manually? No, just rely on outputs
    if n["name"] in ["Clouds2", "BnWSpots", "GrungeDirt", "TileGenerator"]:
        trans_cpp_methods += f'\t\t\t->AddArg(std::make_shared<JobArg_ForceTiling>())\n'
        
    trans_cpp_methods += f'\t\t\t;\n'
    trans_cpp_methods += f'\t\tTiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB {n["name"]}"), &Desc);\n'
    trans_cpp_methods += f'\t\tCycle->AddJob(TargetId, std::move(RenderJob));\n'
    trans_cpp_methods += f'\t\treturn Result;\n\t}}\n'

expr_cpp_evals = ""
for n in nodes:
    expr_cpp_evals += f'\nvoid UTG_Expression_TGMB_{n["name"]}::Evaluate(FTG_EvaluationContext* InContext) {{\n\tSuper::Evaluate(InContext);\n\tOutput = UE::TextureGraphMaterialBridge::FDesignerTransforms::Create{n["name"]}(InContext->Cycle, Output.GetBufferDescriptor(), InContext->TargetId'
    for inp in n["inputs"]:
        parts = inp.split(":")
        var = parts[1] if len(parts) > 1 else "Source"
        expr_cpp_evals += f', {var}.RasterBlob'
    for s in n["settings"]:
        v = s.split(" ")[1].split("=")[0]
        expr_cpp_evals += f', {v}'
    expr_cpp_evals += ');\n}\n'


# Insert BEFORE the search string so we don't mangle it.
inject_in_file(expressions_h, "UCLASS(MinimalAPI)\nclass UTG_Expression_TGMB_TileGenerator", expr_h)
inject_in_file(transforms_h, "class FSH_TGMBTileGenerator", trans_h_classes)
inject_in_file(transforms_h, "static TiledBlobPtr CreateTileGenerator", trans_h_methods)
inject_in_file(transforms_cpp, "IMPLEMENT_GLOBAL_SHADER(FSH_TGMBTileGenerator", trans_cpp_impls)
inject_in_file(transforms_cpp, "TiledBlobPtr FDesignerTransforms::CreateTileGenerator", trans_cpp_methods)
inject_in_file(expressions_cpp, "void UTG_Expression_TGMB_TileGenerator::Evaluate", expr_cpp_evals)

# Also remember Flood Fill update
ff_output_search = r'''	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = ""))
	FTG_Texture Output;'''
ff_output_inject = r'''	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "Flood Fill Data"))
	FTG_Texture FloodFillData;'''
# For this one, we actually want to insert BEFORE the original and KEEP the original (or replace the whole block)
with open(expressions_h, 'r', encoding='utf-8') as f:
    c = f.read()
if ff_output_search in c:
    c = c.replace(ff_output_search, ff_output_inject + "\n" + ff_output_search)
    with open(expressions_h, 'w', encoding='utf-8') as f: f.write(c)

ff_eval_search = r'''	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateFloodFill(
		InContext->Cycle,
		Output.GetBufferDescriptor(),
		InContext->TargetId,
		Input.RasterBlob,
		static_cast<int32>(OutputMode),
		static_cast<int32>(Connectivity),
		Seed,
		Threshold,
		GradientAngle);'''
ff_eval_inject = r'''	FloodFillData = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateFloodFill(
		InContext->Cycle,
		FloodFillData.GetBufferDescriptor(),
		InContext->TargetId,
		Input.RasterBlob,
		2, // OutputMode 2 for data
		static_cast<int32>(Connectivity),
		Seed,
		Threshold,
		GradientAngle);'''
with open(expressions_cpp, 'r', encoding='utf-8') as f:
    c = f.read()
if ff_eval_search in c:
    c = c.replace(ff_eval_search, ff_eval_inject + "\n\n" + ff_eval_search)
    with open(expressions_cpp, 'w', encoding='utf-8') as f: f.write(c)

print("Generation complete")
