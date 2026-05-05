#include "TextureGraphMaterialBridgeDesignerExpressions.h"

#include "TextureGraphMaterialBridgeDesignerTransforms.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TextureGraphMaterialBridgeDesignerExpressions)

namespace
{
	UE::TextureGraphMaterialBridge::FGradientMapSettings BuildGradientMapSettings(const UTG_Expression_TGMB_GradientMap& Expression)
	{
		UE::TextureGraphMaterialBridge::FGradientMapSettings Settings;
		Settings.NumStops = FMath::Clamp(Expression.StopCount, 2, 8);
		Settings.Interpolation = static_cast<int32>(Expression.Interpolation);
		Settings.AddressMode = static_cast<int32>(Expression.AddressMode);

		Settings.Positions[0] = Expression.Position0;
		Settings.Positions[1] = Expression.Position1;
		Settings.Positions[2] = Expression.Position2;
		Settings.Positions[3] = Expression.Position3;
		Settings.Positions[4] = Expression.Position4;
		Settings.Positions[5] = Expression.Position5;
		Settings.Positions[6] = Expression.Position6;
		Settings.Positions[7] = Expression.Position7;

		Settings.Colors[0] = Expression.Color0;
		Settings.Colors[1] = Expression.Color1;
		Settings.Colors[2] = Expression.Color2;
		Settings.Colors[3] = Expression.Color3;
		Settings.Colors[4] = Expression.Color4;
		Settings.Colors[5] = Expression.Color5;
		Settings.Colors[6] = Expression.Color6;
		Settings.Colors[7] = Expression.Color7;

		for (int32 Index = 0; Index < UE_ARRAY_COUNT(Settings.Positions); ++Index)
		{
			Settings.Positions[Index] = FMath::Clamp(Settings.Positions[Index], 0.0f, 1.0f);
		}

		return Settings;
	}
}

void UTG_Expression_TGMB_TileGenerator::Evaluate(FTG_EvaluationContext* InContext)
{
	Super::Evaluate(InContext);

	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateTileGenerator(
		InContext->Cycle,
		Output.GetBufferDescriptor(),
		InContext->TargetId,
		static_cast<int32>(OutputMode),
		static_cast<int32>(Pattern),
		Seed,
		static_cast<float>(CountX),
		static_cast<float>(CountY),
		Scale,
		Spacing,
		Offset.X,
		Offset.Y,
		Rotation,
		PositionJitter,
		SizeJitter,
		RotationJitter,
		LuminanceJitter);
}

void UTG_Expression_TGMB_TileSampler::Evaluate(FTG_EvaluationContext* InContext)
{
	Super::Evaluate(InContext);

	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateTileSampler(
		InContext->Cycle,
		Output.GetBufferDescriptor(),
		InContext->TargetId,
		Source.RasterBlob,
		Mask.RasterBlob,
		ScaleMap.RasterBlob,
		RotationMap.RasterBlob,
		Seed,
		static_cast<float>(CountX),
		static_cast<float>(CountY),
		Scale,
		Spacing,
		Rotation,
		PositionJitter,
		SizeJitter,
		RotationJitter);
}

void UTG_Expression_TGMB_GradientMap::Evaluate(FTG_EvaluationContext* InContext)
{
	Super::Evaluate(InContext);

	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateGradientMap(
		InContext->Cycle,
		Output.GetBufferDescriptor(),
		InContext->TargetId,
		Input.RasterBlob,
		BuildGradientMapSettings(*this));
}

void UTG_Expression_TGMB_NormalCombine::Evaluate(FTG_EvaluationContext* InContext)
{
	Super::Evaluate(InContext);

	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateNormalCombine(
		InContext->Cycle,
		Output.GetBufferDescriptor(),
		InContext->TargetId,
		BaseNormal.RasterBlob,
		DetailNormal.RasterBlob,
		Mask.RasterBlob,
		static_cast<int32>(Mode),
		DetailStrength,
		bInvertGreen);
}

void UTG_Expression_TGMB_FloodFill::Evaluate(FTG_EvaluationContext* InContext)
{
	Super::Evaluate(InContext);

	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateFloodFill(
		InContext->Cycle,
		Output.GetBufferDescriptor(),
		InContext->TargetId,
		Input.RasterBlob,
		static_cast<int32>(OutputMode),
		static_cast<int32>(Connectivity),
		Seed,
		Threshold,
		GradientAngle);
}

void UTG_Expression_TGMB_SlopeBlur::Evaluate(FTG_EvaluationContext* InContext)
{
	Super::Evaluate(InContext);

	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateSlopeBlur(
		InContext->Cycle,
		Output.GetBufferDescriptor(),
		InContext->TargetId,
		Input.RasterBlob,
		Slope.RasterBlob,
		Intensity,
		Samples,
		static_cast<int32>(Mode),
		bInvertSlope);
}

void UTG_Expression_TGMB_Bevel::Evaluate(FTG_EvaluationContext* InContext)
{
	Super::Evaluate(InContext);

	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateBevel(
		InContext->Cycle,
		Output.GetBufferDescriptor(),
		InContext->TargetId,
		Input.RasterBlob,
		Distance,
		Smoothness,
		static_cast<int32>(Mode),
		Threshold);
}

void UTG_Expression_TGMB_AmbientOcclusionHBAO::Evaluate(FTG_EvaluationContext* InContext)
{
	Super::Evaluate(InContext);

	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateHBAO(
		InContext->Cycle,
		Output.GetBufferDescriptor(),
		InContext->TargetId,
		Height.RasterBlob,
		Radius,
		Samples,
		HeightScale,
		Bias,
		Contrast,
		bInvert);
}

void UTG_Expression_TGMB_CurvatureSmooth::Evaluate(FTG_EvaluationContext* InContext)
{
	Super::Evaluate(InContext);

	Curvature = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateCurvatureSmooth(
		InContext->Cycle,
		Curvature.GetBufferDescriptor(),
		InContext->TargetId,
		Normal.RasterBlob,
		0,
		Radius,
		Smoothing,
		Intensity);

	Convex = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateCurvatureSmooth(
		InContext->Cycle,
		Convex.GetBufferDescriptor(),
		InContext->TargetId,
		Normal.RasterBlob,
		1,
		Radius,
		Smoothing,
		Intensity);

	Concave = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateCurvatureSmooth(
		InContext->Cycle,
		Concave.GetBufferDescriptor(),
		InContext->TargetId,
		Normal.RasterBlob,
		2,
		Radius,
		Smoothing,
		Intensity);
}

void UTG_Expression_TGMB_MultiMaterialBlend::Evaluate(FTG_EvaluationContext* InContext)
{
	Super::Evaluate(InContext);

	BaseColor = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateMultiMaterialBlend(
		InContext->Cycle,
		BaseColor.GetBufferDescriptor(),
		InContext->TargetId,
		ABaseColor.RasterBlob,
		ANormal.RasterBlob,
		AORM.RasterBlob,
		AHeight.RasterBlob,
		BBaseColor.RasterBlob,
		BNormal.RasterBlob,
		BORM.RasterBlob,
		BHeight.RasterBlob,
		Mask.RasterBlob,
		0,
		NormalStrength);

	Normal = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateMultiMaterialBlend(
		InContext->Cycle,
		Normal.GetBufferDescriptor(),
		InContext->TargetId,
		ABaseColor.RasterBlob,
		ANormal.RasterBlob,
		AORM.RasterBlob,
		AHeight.RasterBlob,
		BBaseColor.RasterBlob,
		BNormal.RasterBlob,
		BORM.RasterBlob,
		BHeight.RasterBlob,
		Mask.RasterBlob,
		1,
		NormalStrength);

	ORM = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateMultiMaterialBlend(
		InContext->Cycle,
		ORM.GetBufferDescriptor(),
		InContext->TargetId,
		ABaseColor.RasterBlob,
		ANormal.RasterBlob,
		AORM.RasterBlob,
		AHeight.RasterBlob,
		BBaseColor.RasterBlob,
		BNormal.RasterBlob,
		BORM.RasterBlob,
		BHeight.RasterBlob,
		Mask.RasterBlob,
		2,
		NormalStrength);

	Height = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateMultiMaterialBlend(
		InContext->Cycle,
		Height.GetBufferDescriptor(),
		InContext->TargetId,
		ABaseColor.RasterBlob,
		ANormal.RasterBlob,
		AORM.RasterBlob,
		AHeight.RasterBlob,
		BBaseColor.RasterBlob,
		BNormal.RasterBlob,
		BORM.RasterBlob,
		BHeight.RasterBlob,
		Mask.RasterBlob,
		3,
		NormalStrength);
}
