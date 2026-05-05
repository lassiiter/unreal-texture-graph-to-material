#include "TextureGraphMaterialBridgeDesignerTransforms.h"

#include "2D/TextureHelper.h"
#include "FxMat/MaterialManager.h"
#include "Job/JobArgs.h"
#include "Job/JobBatch.h"
#include "TextureGraphEngine.h"
#include "Transform/Utility/T_CombineTiledBlob.h"

IMPLEMENT_GLOBAL_SHADER(FSH_TGMBTileGenerator, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBTileGenerator", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBTileSampler, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBTileSampler", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBGradientMap, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBGradientMap", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBNormalCombine, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBNormalCombine", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBFloodFill, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBFloodFill", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBSlopeBlur, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBSlopeBlur", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBBevel, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBBevel", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBHBAO, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBHBAO", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBCurvatureSmooth, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBCurvatureSmooth", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBMultiMaterialBlend, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBMultiMaterialBlend", SF_Pixel);

namespace UE::TextureGraphMaterialBridge
{
	namespace
	{
		constexpr int32 DefaultTextureSize = 1024;

		BufferDescriptor BuildOutputDesc(BufferDescriptor DesiredDesc, TiledBlobPtr Reference = nullptr, int32 DefaultItemsPerPoint = 4)
		{
			BufferDescriptor Desc = Reference ? BufferDescriptor::Combine(Reference->GetDescriptor(), DesiredDesc) : DesiredDesc;

			if (Desc.Width <= 0)
			{
				Desc.Width = Reference ? Reference->GetWidth() : DefaultTextureSize;
			}

			if (Desc.Height <= 0)
			{
				Desc.Height = Reference ? Reference->GetHeight() : DefaultTextureSize;
			}

			if (Desc.Format == BufferFormat::Auto)
			{
				Desc.Format = BufferFormat::Byte;
			}

			if (Desc.ItemsPerPoint <= 0)
			{
				Desc.ItemsPerPoint = DefaultItemsPerPoint;
			}

			Desc.DefaultValue = FLinearColor::Black;
			return Desc;
		}

		TiledBlobPtr CombineIfNeeded(MixUpdateCyclePtr Cycle, int32 TargetId, TiledBlobPtr Source)
		{
			if (!Source)
			{
				return nullptr;
			}

			return T_CombineTiledBlob::Create(Cycle, Source->GetDescriptor(), TargetId, Source);
		}

		template <typename ShaderType>
		JobUPtr CreateShaderJob(MixUpdateCyclePtr Cycle, int32 TargetId, const TCHAR* Name)
		{
			const RenderMaterial_FXPtr RenderMaterial = TextureGraphEngine::GetMaterialManager()->CreateMaterial_FX<VSH_Simple, ShaderType>(Name);
			check(RenderMaterial);
			return std::make_unique<Job>(Cycle->GetMix(), TargetId, std::static_pointer_cast<BlobTransform>(RenderMaterial));
		}
	}

	TiledBlobPtr FDesignerTransforms::CreateTileGenerator(
		MixUpdateCyclePtr Cycle,
		BufferDescriptor DesiredDesc,
		int32 TargetId,
		int32 OutputMode,
		int32 PatternType,
		int32 Seed,
		float CountX,
		float CountY,
		float Scale,
		float Spacing,
		float OffsetX,
		float OffsetY,
		float Rotation,
		float PositionJitter,
		float SizeJitter,
		float RotationJitter,
		float LuminanceJitter)
	{
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc);
		FTileInfo TileInfo;

		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBTileGenerator>(Cycle, TargetId, TEXT("TGMB_TileGenerator"));
		RenderJob
			->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_INT(OutputMode, "OutputMode"))
			->AddArg(ARG_INT(PatternType, "PatternType"))
			->AddArg(ARG_INT(Seed, "Seed"))
			->AddArg(ARG_FLOAT(CountX, "CountX"))
			->AddArg(ARG_FLOAT(CountY, "CountY"))
			->AddArg(ARG_FLOAT(Scale, "Scale"))
			->AddArg(ARG_FLOAT(Spacing, "Spacing"))
			->AddArg(ARG_FLOAT(OffsetX, "OffsetX"))
			->AddArg(ARG_FLOAT(OffsetY, "OffsetY"))
			->AddArg(ARG_FLOAT(Rotation, "Rotation"))
			->AddArg(ARG_FLOAT(PositionJitter, "PositionJitter"))
			->AddArg(ARG_FLOAT(SizeJitter, "SizeJitter"))
			->AddArg(ARG_FLOAT(RotationJitter, "RotationJitter"))
			->AddArg(ARG_FLOAT(LuminanceJitter, "LuminanceJitter"))
			->AddArg(std::make_shared<JobArg_ForceTiling>());

		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB Tile Generator"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateTileSampler(
		MixUpdateCyclePtr Cycle,
		BufferDescriptor DesiredDesc,
		int32 TargetId,
		TiledBlobPtr Source,
		TiledBlobPtr Mask,
		TiledBlobPtr ScaleMap,
		TiledBlobPtr RotationMap,
		int32 Seed,
		float CountX,
		float CountY,
		float Scale,
		float Spacing,
		float Rotation,
		float PositionJitter,
		float SizeJitter,
		float RotationJitter)
	{
		if (!Source)
		{
			return TextureHelper::GetBlack();
		}

		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source);
		TiledBlobPtr CombinedMask = CombineIfNeeded(Cycle, TargetId, Mask ? Mask : TextureHelper::GetWhite());
		TiledBlobPtr CombinedScaleMap = CombineIfNeeded(Cycle, TargetId, ScaleMap ? ScaleMap : TextureHelper::GetWhite());
		TiledBlobPtr CombinedRotationMap = CombineIfNeeded(Cycle, TargetId, RotationMap ? RotationMap : TextureHelper::GetBlack());
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, Source);
		FTileInfo TileInfo;

		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBTileSampler>(Cycle, TargetId, TEXT("TGMB_TileSampler"));
		RenderJob
			->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_BLOB(CombinedMask, "MaskTexture"))
			->AddArg(ARG_BLOB(CombinedScaleMap, "ScaleMap"))
			->AddArg(ARG_BLOB(CombinedRotationMap, "RotationMap"))
			->AddArg(ARG_INT(Seed, "Seed"))
			->AddArg(ARG_FLOAT(CountX, "CountX"))
			->AddArg(ARG_FLOAT(CountY, "CountY"))
			->AddArg(ARG_FLOAT(Scale, "Scale"))
			->AddArg(ARG_FLOAT(Spacing, "Spacing"))
			->AddArg(ARG_FLOAT(Rotation, "Rotation"))
			->AddArg(ARG_FLOAT(PositionJitter, "PositionJitter"))
			->AddArg(ARG_FLOAT(SizeJitter, "SizeJitter"))
			->AddArg(ARG_FLOAT(RotationJitter, "RotationJitter"));

		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB Tile Sampler"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateGradientMap(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, const FGradientMapSettings& Settings)
	{
		if (!Source)
		{
			return TextureHelper::GetBlack();
		}

		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source);
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, Source);

		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBGradientMap>(Cycle, TargetId, TEXT("TGMB_GradientMap"));
		RenderJob
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_INT(FMath::Clamp(Settings.NumStops, 2, 8), "NumStops"))
			->AddArg(ARG_INT(Settings.Interpolation, "Interpolation"))
			->AddArg(ARG_INT(Settings.AddressMode, "AddressMode"))
			->AddArg(ARG_FLOAT(Settings.Positions[0], "Position0"))
			->AddArg(ARG_FLOAT(Settings.Positions[1], "Position1"))
			->AddArg(ARG_FLOAT(Settings.Positions[2], "Position2"))
			->AddArg(ARG_FLOAT(Settings.Positions[3], "Position3"))
			->AddArg(ARG_FLOAT(Settings.Positions[4], "Position4"))
			->AddArg(ARG_FLOAT(Settings.Positions[5], "Position5"))
			->AddArg(ARG_FLOAT(Settings.Positions[6], "Position6"))
			->AddArg(ARG_FLOAT(Settings.Positions[7], "Position7"))
			->AddArg(ARG_LINEAR_COLOR(Settings.Colors[0], "Color0"))
			->AddArg(ARG_LINEAR_COLOR(Settings.Colors[1], "Color1"))
			->AddArg(ARG_LINEAR_COLOR(Settings.Colors[2], "Color2"))
			->AddArg(ARG_LINEAR_COLOR(Settings.Colors[3], "Color3"))
			->AddArg(ARG_LINEAR_COLOR(Settings.Colors[4], "Color4"))
			->AddArg(ARG_LINEAR_COLOR(Settings.Colors[5], "Color5"))
			->AddArg(ARG_LINEAR_COLOR(Settings.Colors[6], "Color6"))
			->AddArg(ARG_LINEAR_COLOR(Settings.Colors[7], "Color7"));

		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB Gradient Map"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateNormalCombine(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr BaseNormal, TiledBlobPtr DetailNormal, TiledBlobPtr Mask, int32 BlendMode, float DetailStrength, bool bInvertGreen)
	{
		if (!BaseNormal)
		{
			return DetailNormal ? DetailNormal : TextureHelper::GetBlack();
		}

		if (!DetailNormal || DetailStrength <= KINDA_SMALL_NUMBER)
		{
			return BaseNormal;
		}

		TiledBlobPtr CombinedBase = CombineIfNeeded(Cycle, TargetId, BaseNormal);
		TiledBlobPtr CombinedDetail = CombineIfNeeded(Cycle, TargetId, DetailNormal ? DetailNormal : TextureHelper::GetBlack());
		TiledBlobPtr CombinedMask = CombineIfNeeded(Cycle, TargetId, Mask ? Mask : TextureHelper::GetWhite());
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, BaseNormal);

		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBNormalCombine>(Cycle, TargetId, TEXT("TGMB_NormalCombine"));
		RenderJob
			->AddArg(ARG_BLOB(CombinedBase, "BaseNormal"))
			->AddArg(ARG_BLOB(CombinedDetail, "DetailNormal"))
			->AddArg(ARG_BLOB(CombinedMask, "MaskTexture"))
			->AddArg(ARG_INT(BlendMode, "BlendMode"))
			->AddArg(ARG_FLOAT(DetailStrength, "DetailStrength"))
			->AddArg(ARG_FLOAT(bInvertGreen ? 1.0f : 0.0f, "InvertGreen"));

		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB Normal Combine"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateFloodFill(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, int32 OutputMode, int32 Connectivity, int32 Seed, float Threshold, float GradientAngle)
	{
		if (!Source)
		{
			return TextureHelper::GetBlack();
		}

		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source);
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, Source);
		FTileInfo TileInfo;

		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBFloodFill>(Cycle, TargetId, TEXT("TGMB_FloodFill"));
		RenderJob
			->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_INT(OutputMode, "OutputMode"))
			->AddArg(ARG_INT(Connectivity, "Connectivity"))
			->AddArg(ARG_INT(Seed, "Seed"))
			->AddArg(ARG_FLOAT(Threshold, "Threshold"))
			->AddArg(ARG_FLOAT(GradientAngle, "GradientAngle"));

		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB Flood Fill"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateSlopeBlur(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, TiledBlobPtr Slope, float Intensity, int32 Samples, int32 Mode, bool bInvertSlope)
	{
		if (!Source)
		{
			return TextureHelper::GetBlack();
		}

		if (!Slope || Intensity <= KINDA_SMALL_NUMBER || Samples <= 0)
		{
			return Source;
		}

		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source);
		TiledBlobPtr CombinedSlope = CombineIfNeeded(Cycle, TargetId, Slope);
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, Source);
		FTileInfo TileInfo;

		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBSlopeBlur>(Cycle, TargetId, TEXT("TGMB_SlopeBlur"));
		RenderJob
			->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_BLOB(CombinedSlope, "SlopeTexture"))
			->AddArg(ARG_INT(FMath::Clamp(Samples, 1, 64), "Samples"))
			->AddArg(ARG_INT(Mode, "Mode"))
			->AddArg(ARG_FLOAT(Intensity, "Intensity"))
			->AddArg(ARG_FLOAT(bInvertSlope ? 1.0f : 0.0f, "InvertSlope"));

		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB Slope Blur"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateBevel(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, float Distance, float Smoothness, int32 Mode, float Threshold)
	{
		if (!Source)
		{
			return TextureHelper::GetBlack();
		}

		if (Distance <= KINDA_SMALL_NUMBER)
		{
			return Source;
		}

		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source);
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, Source);
		FTileInfo TileInfo;

		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBBevel>(Cycle, TargetId, TEXT("TGMB_Bevel"));
		RenderJob
			->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_INT(Mode, "Mode"))
			->AddArg(ARG_FLOAT(Distance, "Distance"))
			->AddArg(ARG_FLOAT(Smoothness, "Smoothness"))
			->AddArg(ARG_FLOAT(Threshold, "Threshold"));

		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB Bevel"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateHBAO(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, float Radius, int32 Samples, float HeightScale, float Bias, float Contrast, bool bInvert)
	{
		if (!Source)
		{
			return TextureHelper::GetWhite();
		}

		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source);
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, Source);
		FTileInfo TileInfo;

		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBHBAO>(Cycle, TargetId, TEXT("TGMB_HBAO"));
		RenderJob
			->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_INT(FMath::Clamp(Samples, 1, 64), "Samples"))
			->AddArg(ARG_FLOAT(Radius, "Radius"))
			->AddArg(ARG_FLOAT(HeightScale, "HeightScale"))
			->AddArg(ARG_FLOAT(Bias, "Bias"))
			->AddArg(ARG_FLOAT(Contrast, "Contrast"))
			->AddArg(ARG_FLOAT(bInvert ? 1.0f : 0.0f, "Invert"));

		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB HBAO"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateCurvatureSmooth(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, int32 OutputMode, float Radius, float Smoothing, float Intensity)
	{
		if (!Source)
		{
			return TextureHelper::GetBlack();
		}

		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source);
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, Source);
		FTileInfo TileInfo;

		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBCurvatureSmooth>(Cycle, TargetId, TEXT("TGMB_CurvatureSmooth"));
		RenderJob
			->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_INT(OutputMode, "OutputMode"))
			->AddArg(ARG_FLOAT(Radius, "Radius"))
			->AddArg(ARG_FLOAT(Smoothing, "Smoothing"))
			->AddArg(ARG_FLOAT(Intensity, "Intensity"));

		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB Curvature Smooth"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateMultiMaterialBlend(
		MixUpdateCyclePtr Cycle,
		BufferDescriptor DesiredDesc,
		int32 TargetId,
		TiledBlobPtr ABaseColor,
		TiledBlobPtr ANormal,
		TiledBlobPtr AORM,
		TiledBlobPtr AHeight,
		TiledBlobPtr BBaseColor,
		TiledBlobPtr BNormal,
		TiledBlobPtr BORM,
		TiledBlobPtr BHeight,
		TiledBlobPtr Mask,
		int32 OutputMode,
		float NormalStrength)
	{
		TiledBlobPtr Reference = ABaseColor ? ABaseColor : BBaseColor;
		if (!Reference)
		{
			return TextureHelper::GetBlack();
		}

		TiledBlobPtr CombinedABaseColor = CombineIfNeeded(Cycle, TargetId, ABaseColor ? ABaseColor : TextureHelper::GetBlack());
		TiledBlobPtr CombinedANormal = CombineIfNeeded(Cycle, TargetId, ANormal ? ANormal : TextureHelper::GetBlack());
		TiledBlobPtr CombinedAORM = CombineIfNeeded(Cycle, TargetId, AORM ? AORM : TextureHelper::GetBlack());
		TiledBlobPtr CombinedAHeight = CombineIfNeeded(Cycle, TargetId, AHeight ? AHeight : TextureHelper::GetBlack());
		TiledBlobPtr CombinedBBaseColor = CombineIfNeeded(Cycle, TargetId, BBaseColor ? BBaseColor : TextureHelper::GetBlack());
		TiledBlobPtr CombinedBNormal = CombineIfNeeded(Cycle, TargetId, BNormal ? BNormal : TextureHelper::GetBlack());
		TiledBlobPtr CombinedBORM = CombineIfNeeded(Cycle, TargetId, BORM ? BORM : TextureHelper::GetBlack());
		TiledBlobPtr CombinedBHeight = CombineIfNeeded(Cycle, TargetId, BHeight ? BHeight : TextureHelper::GetBlack());
		TiledBlobPtr CombinedMask = CombineIfNeeded(Cycle, TargetId, Mask ? Mask : TextureHelper::GetBlack());
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, Reference);

		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBMultiMaterialBlend>(Cycle, TargetId, TEXT("TGMB_MultiMaterialBlend"));
		RenderJob
			->AddArg(ARG_BLOB(CombinedABaseColor, "ABaseColor"))
			->AddArg(ARG_BLOB(CombinedANormal, "ANormal"))
			->AddArg(ARG_BLOB(CombinedAORM, "AORM"))
			->AddArg(ARG_BLOB(CombinedAHeight, "AHeight"))
			->AddArg(ARG_BLOB(CombinedBBaseColor, "BBaseColor"))
			->AddArg(ARG_BLOB(CombinedBNormal, "BNormal"))
			->AddArg(ARG_BLOB(CombinedBORM, "BORM"))
			->AddArg(ARG_BLOB(CombinedBHeight, "BHeight"))
			->AddArg(ARG_BLOB(CombinedMask, "MaskTexture"))
			->AddArg(ARG_INT(OutputMode, "OutputMode"))
			->AddArg(ARG_FLOAT(NormalStrength, "NormalStrength"));

		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB Multi-Material Blend"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}
}
