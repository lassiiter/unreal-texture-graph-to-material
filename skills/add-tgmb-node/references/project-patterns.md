# TextureGraphMaterialBridge Node Patterns

## Naming

- Expression class: `UTG_Expression_TGMB_<Suffix>`
- Shader wrapper: `FSH_TGMB<Suffix>`
- Transform function: `FDesignerTransforms::Create<Suffix>`
- Shader function: `FSH_TGMB<Suffix>`
- Render material name: `TEXT("TGMB_<Suffix>")`
- Result name: `TEXT("TGMB Display Name")`

Use PascalCase for C++ suffixes and readable title text for `GetDefaultName()`.

## Expression Class Pattern

Add expression classes to `TextureGraphMaterialBridgeDesignerExpressions.h`.

```cpp
UCLASS(MinimalAPI)
class UTG_Expression_TGMB_Example : public UTG_Expression
{
	GENERATED_BODY()

public:
	TG_DECLARE_EXPRESSION(TG_Category::Filter);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Example"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Describe the node.")); }

	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Source"))
	FTG_Texture Source;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1"))
	float Amount = 0.5f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = ""))
	FTG_Texture Output;
};
```

Implement `Evaluate` in `TextureGraphMaterialBridgeDesignerExpressions.cpp`.

```cpp
void UTG_Expression_TGMB_Example::Evaluate(FTG_EvaluationContext* InContext)
{
	Super::Evaluate(InContext);

	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateExample(
		InContext->Cycle,
		Output.GetBufferDescriptor(),
		InContext->TargetId,
		Source.RasterBlob,
		Amount);
}
```

## Transform Pattern

Declare the shader class and transform function in `TextureGraphMaterialBridgeDesignerTransforms.h`.

```cpp
class FSH_TGMBExample : public FSH_Base
{
public:
	DECLARE_EXPORTED_GLOBAL_SHADER(FSH_TGMBExample, UE_API);
	SHADER_USE_PARAMETER_STRUCT(FSH_TGMBExample, FSH_Base);
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FTileInfo, TileInfo)
		SHADER_PARAMETER_STRUCT(FStandardSamplerStates, SamplerStates)
		SHADER_PARAMETER_TEXTURE(Texture2D, SourceTexture)
		SHADER_PARAMETER(float, Amount)
	END_SHADER_PARAMETER_STRUCT()
	TEXTURE_ENGINE_DEFAULT_PERMUTATION;
	TEXTUREGRAPH_ENGINE_DEFAULT_COMPILATION_ENV;
};
```

In the `FDesignerTransforms` class declaration:

```cpp
static TiledBlobPtr CreateExample(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, float Amount);
```

Implement in `TextureGraphMaterialBridgeDesignerTransforms.cpp`.

```cpp
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBExample, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBExample", SF_Pixel);

TiledBlobPtr FDesignerTransforms::CreateExample(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, float Amount)
{
	if (!Source)
	{
		return TextureHelper::GetBlack();
	}

	TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source);
	BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, Source);
	FTileInfo TileInfo;

	JobUPtr RenderJob = CreateShaderJob<FSH_TGMBExample>(Cycle, TargetId, TEXT("TGMB_Example"));
	RenderJob
		->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
		->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
		->AddArg(ARG_FLOAT(Amount, "Amount"));

	TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB Example"), &Desc);
	Cycle->AddJob(TargetId, std::move(RenderJob));
	return Result;
}
```

## Shader Pattern

Add global parameters to `TGMB_DesignerNodes.usf` only if no existing global already matches the name. Then add the pixel shader function.

```hlsl
float4 FSH_TGMBExample(float2 UV : TEXCOORD0) : SV_Target0
{
	float2 LayerUV = TileInfo_fromCurrentTileToLayer(UV);
	float4 Source = SourceTexture.Sample(SamplerStates_Linear_Wrap, LayerUV);
	return saturate(Source * Amount);
}
```

Use helpers in `TGMB_DesignerCommon.ush` for luma, hash, noise, rotation, normal encode/decode, cells, and Sobel operations. Add new helpers there only when at least two shaders can share them.

## Common Choices

- Category: `TG_Category::Procedural` for generators, `TG_Category::Filter` for image filters/warps, `TG_Category::Adjustment` for color/normal/material adjustments.
- Boolean settings cross into shaders as `float` `0.0f` or `1.0f` unless an existing pattern uses `int32`.
- Enum settings should be `UENUM(BlueprintType)` in the expression header and cast to `int32` in `Evaluate`.
- Multi-output expressions should call one transform per output using an `OutputMode` or mode parameter, matching `FloodFill` and `CurvatureSmooth`.
- Clamp user-facing settings with `UIMin`, `ClampMin`, `UIMax`, and `ClampMax` when the shader assumes a range.
