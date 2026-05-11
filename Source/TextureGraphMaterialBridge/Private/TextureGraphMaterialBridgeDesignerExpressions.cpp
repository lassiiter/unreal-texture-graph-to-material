#include "TextureGraphMaterialBridgeDesignerExpressions.h"

#if TGMB_WITH_DESIGNER_NODES
#include "TextureGraphMaterialBridgeDesignerTransforms.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(TextureGraphMaterialBridgeDesignerExpressions)

#if TGMB_WITH_DESIGNER_NODES
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

#endif

#if TGMB_WITH_DESIGNER_NODES
void UTG_Expression_TGMB_FloodFillToRandomGrayscale::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateFloodFillToRandomGrayscale(InContext->Cycle, Output.GetBufferDescriptor(), InContext->TargetId, Source.RasterBlob, Seed);
}

void UTG_Expression_TGMB_FloodFillToRandomColor::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateFloodFillToRandomColor(InContext->Cycle, Output.GetBufferDescriptor(), InContext->TargetId, Source.RasterBlob, Seed);
}

void UTG_Expression_TGMB_FloodFillToGradient::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateFloodFillToGradient(InContext->Cycle, Output.GetBufferDescriptor(), InContext->TargetId, Source.RasterBlob, GradientAngle, RotationJitter, Seed);
}

void UTG_Expression_TGMB_FloodFillToPosition::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateFloodFillToPosition(InContext->Cycle, Output.GetBufferDescriptor(), InContext->TargetId, Source.RasterBlob);
}

void UTG_Expression_TGMB_FloodFillToBBoxSize::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateFloodFillToBBoxSize(InContext->Cycle, Output.GetBufferDescriptor(), InContext->TargetId, Source.RasterBlob, static_cast<int32>(OutputMode));
}

void UTG_Expression_TGMB_FloodFillMapper::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateFloodFillMapper(InContext->Cycle, Output.GetBufferDescriptor(), InContext->TargetId, Source.RasterBlob, MaskTexture.RasterBlob);
}

void UTG_Expression_TGMB_MultiDirectionalWarp::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateMultiDirectionalWarp(InContext->Cycle, Output.GetBufferDescriptor(), InContext->TargetId, Source.RasterBlob, MaskTexture.RasterBlob, Intensity, Rotation, GradientAngle);
}

void UTG_Expression_TGMB_NonUniformDirectionalWarp::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateNonUniformDirectionalWarp(InContext->Cycle, Output.GetBufferDescriptor(), InContext->TargetId, Source.RasterBlob, MaskTexture.RasterBlob, Intensity, GradientAngle);
}

void UTG_Expression_TGMB_DirectionalDistance::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateDirectionalDistance(InContext->Cycle, Output.GetBufferDescriptor(), InContext->TargetId, Source.RasterBlob, Distance, GradientAngle, Samples, Threshold);
}

void UTG_Expression_TGMB_ShapeSplatter::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateShapeSplatter(InContext->Cycle, Output.GetBufferDescriptor(), InContext->TargetId, Source.RasterBlob, MaskTexture.RasterBlob, CountX, CountY, Scale, Threshold);
}

void UTG_Expression_TGMB_Cells1::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateCells1(
		InContext->Cycle,
		Output.GetBufferDescriptor(),
		InContext->TargetId,
		Scale,
		Disorder,
		DisorderSpeed,
		DisorderAnisotropy,
		DisorderAnisotropyAngle,
		static_cast<int32>(Pattern),
		PatternSize.X,
		PatternSize.Y,
		PatternScale,
		LuminanceRandom,
		Angle,
		AngleRandom,
		TileOffset.X,
		TileOffset.Y,
		bNonSquareExpansion,
		Seed);
}

void UTG_Expression_TGMB_Cells2::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateCells2(
		InContext->Cycle,
		Output.GetBufferDescriptor(),
		InContext->TargetId,
		Scale,
		EdgeWidth,
		bInvert,
		Disorder,
		DisorderSpeed,
		TileOffset.X,
		TileOffset.Y,
		bNonSquareExpansion,
		Seed);
}

void UTG_Expression_TGMB_Cells3::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateCells3(
		InContext->Cycle,
		Output.GetBufferDescriptor(),
		InContext->TargetId,
		Scale,
		Hardness,
		bInvert,
		Disorder,
		DisorderSpeed,
		DisorderAnisotropy,
		DisorderAnisotropyAngle,
		PatternSize.X,
		PatternSize.Y,
		PatternScale,
		Angle,
		AngleRandom,
		TileOffset.X,
		TileOffset.Y,
		bNonSquareExpansion,
		Seed);
}

void UTG_Expression_TGMB_Cells4::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateCells4(
		InContext->Cycle,
		Output.GetBufferDescriptor(),
		InContext->TargetId,
		Input.RasterBlob,
		Scale,
		Disorder,
		DisorderSpeed,
		static_cast<int32>(ColorSource),
		PseudorandomSeed,
		TileOffset.X,
		TileOffset.Y,
		bNonSquareExpansion,
		Seed);
}

void UTG_Expression_TGMB_Clouds2::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateClouds2(InContext->Cycle, Output.GetBufferDescriptor(), InContext->TargetId, Scale, Contrast, Bias, OffsetX, OffsetY, Seed);
}

void UTG_Expression_TGMB_BnWSpots::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateBnWSpots(InContext->Cycle, Output.GetBufferDescriptor(), InContext->TargetId, Scale, Threshold, Smoothness, OffsetX, OffsetY, Seed);
}

void UTG_Expression_TGMB_GrungeDirt::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateGrungeDirt(InContext->Cycle, Output.GetBufferDescriptor(), InContext->TargetId, Scale, Contrast, Bias, OffsetX, OffsetY, Seed);
}

void UTG_Expression_TGMB_Highpass::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateHighpass(InContext->Cycle, Output.GetBufferDescriptor(), InContext->TargetId, Source.RasterBlob, Radius, Contrast);
}

void UTG_Expression_TGMB_LuminanceHighpass::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateLuminanceHighpass(InContext->Cycle, Output.GetBufferDescriptor(), InContext->TargetId, Source.RasterBlob, Radius, Contrast);
}

void UTG_Expression_TGMB_BlurHQ::Evaluate(FTG_EvaluationContext* InContext)
{
	Super::Evaluate(InContext);

	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateBlurHQ(
		InContext->Cycle,
		Output.GetBufferDescriptor(),
		InContext->TargetId,
		Input.RasterBlob,
		Intensity,
		Quality);
}

void UTG_Expression_TGMB_CurvatureSobel::Evaluate(FTG_EvaluationContext* InContext) {
	Super::Evaluate(InContext);
	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateCurvatureSobel(InContext->Cycle, Output.GetBufferDescriptor(), InContext->TargetId, Source.RasterBlob, static_cast<int32>(OutputMode), Radius, Intensity, Threshold);
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

	FloodFillData = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateFloodFillData(
		InContext->Cycle,
		FloodFillData.GetBufferDescriptor(),
		InContext->TargetId,
		Input.RasterBlob,
		static_cast<int32>(Connectivity),
		Threshold);

	Output = UE::TextureGraphMaterialBridge::FDesignerTransforms::CreateFloodFill(
		InContext->Cycle,
		Output.GetBufferDescriptor(),
		InContext->TargetId,
		FloodFillData,
		static_cast<int32>(OutputMode),
		Seed,
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
#else
namespace
{
	void ClearTextureOutput(FTG_Texture& Texture)
	{
		Texture = FTG_Texture();
	}
}

#define TGMB_EMPTY_OUTPUT_EVALUATE(ExpressionType, OutputMember) \
	void ExpressionType::Evaluate(FTG_EvaluationContext* InContext) \
	{ \
		Super::Evaluate(InContext); \
		ClearTextureOutput(OutputMember); \
	}

TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_FloodFillToRandomGrayscale, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_FloodFillToRandomColor, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_FloodFillToGradient, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_FloodFillToPosition, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_FloodFillToBBoxSize, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_FloodFillMapper, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_MultiDirectionalWarp, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_NonUniformDirectionalWarp, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_DirectionalDistance, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_ShapeSplatter, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_Cells1, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_Cells2, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_Cells3, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_Cells4, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_Clouds2, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_BnWSpots, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_GrungeDirt, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_Highpass, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_LuminanceHighpass, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_BlurHQ, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_CurvatureSobel, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_TileGenerator, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_TileSampler, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_GradientMap, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_NormalCombine, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_SlopeBlur, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_Bevel, Output)
TGMB_EMPTY_OUTPUT_EVALUATE(UTG_Expression_TGMB_AmbientOcclusionHBAO, Output)

#undef TGMB_EMPTY_OUTPUT_EVALUATE

void UTG_Expression_TGMB_FloodFill::Evaluate(FTG_EvaluationContext* InContext)
{
	Super::Evaluate(InContext);
	ClearTextureOutput(FloodFillData);
	ClearTextureOutput(Output);
}

void UTG_Expression_TGMB_CurvatureSmooth::Evaluate(FTG_EvaluationContext* InContext)
{
	Super::Evaluate(InContext);
	ClearTextureOutput(Curvature);
	ClearTextureOutput(Convex);
	ClearTextureOutput(Concave);
}

void UTG_Expression_TGMB_MultiMaterialBlend::Evaluate(FTG_EvaluationContext* InContext)
{
	Super::Evaluate(InContext);
	ClearTextureOutput(BaseColor);
	ClearTextureOutput(Normal);
	ClearTextureOutput(ORM);
	ClearTextureOutput(Height);
}
#endif
