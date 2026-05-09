#include "TextureGraphMaterialBridgeDesignerTransforms.h"

#include "2D/TextureHelper.h"
#include "FxMat/MaterialManager.h"
#include "Job/JobArgs.h"
#include "Job/JobBatch.h"
#include "TextureGraphEngine.h"
#include "Transform/Utility/T_CombineTiledBlob.h"

IMPLEMENT_GLOBAL_SHADER(FSH_TGMBFloodFillToRandomGrayscale, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBFloodFillToRandomGrayscale", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBFloodFillToRandomColor, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBFloodFillToRandomColor", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBFloodFillToGradient, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBFloodFillToGradient", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBFloodFillToPosition, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBFloodFillToPosition", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBFloodFillMapper, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBFloodFillMapper", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBMultiDirectionalWarp, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBMultiDirectionalWarp", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBNonUniformDirectionalWarp, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBNonUniformDirectionalWarp", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBDirectionalDistance, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBDirectionalDistance", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBShapeSplatter, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBShapeSplatter", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBCells1, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBCells1", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBCells2, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBCells2", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBCells3, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBCells3", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBCells4, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBCells4", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBClouds2, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBClouds2", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBBnWSpots, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBBnWSpots", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBGrungeDirt, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBGrungeDirt", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBHighpass, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBHighpass", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBLuminanceHighpass, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBLuminanceHighpass", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBBlurHQ, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBBlurHQ", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBCurvatureSobel, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBCurvatureSobel", SF_Pixel);

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
		constexpr int32 CellsRenderVersion = 10;

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

	
	TiledBlobPtr FDesignerTransforms::CreateFloodFillToRandomGrayscale(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, int32 Seed) {
		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source ? Source : TextureHelper::GetBlack());
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, CombinedSource);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBFloodFillToRandomGrayscale>(Cycle, TargetId, TEXT("TGMB_FloodFillToRandomGrayscale"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_INT(Seed, "Seed"))
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB FloodFillToRandomGrayscale"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateFloodFillToRandomColor(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, int32 Seed) {
		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source ? Source : TextureHelper::GetBlack());
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, CombinedSource);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBFloodFillToRandomColor>(Cycle, TargetId, TEXT("TGMB_FloodFillToRandomColor"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_INT(Seed, "Seed"))
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB FloodFillToRandomColor"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateFloodFillToGradient(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, float GradientAngle, float RotationJitter, int32 Seed) {
		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source ? Source : TextureHelper::GetBlack());
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, CombinedSource);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBFloodFillToGradient>(Cycle, TargetId, TEXT("TGMB_FloodFillToGradient"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_FLOAT(GradientAngle, "GradientAngle"))
			->AddArg(ARG_FLOAT(RotationJitter, "RotationJitter"))
			->AddArg(ARG_INT(Seed, "Seed"))
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB FloodFillToGradient"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateFloodFillToPosition(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source) {
		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source ? Source : TextureHelper::GetBlack());
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, CombinedSource);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBFloodFillToPosition>(Cycle, TargetId, TEXT("TGMB_FloodFillToPosition"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB FloodFillToPosition"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateFloodFillMapper(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, TiledBlobPtr MaskTexture) {
		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source ? Source : TextureHelper::GetBlack());
		TiledBlobPtr CombinedMaskTexture = CombineIfNeeded(Cycle, TargetId, MaskTexture ? MaskTexture : TextureHelper::GetBlack());
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, CombinedSource);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBFloodFillMapper>(Cycle, TargetId, TEXT("TGMB_FloodFillMapper"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_BLOB(CombinedMaskTexture, "MaskTexture"))
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB FloodFillMapper"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateMultiDirectionalWarp(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, TiledBlobPtr MaskTexture, float Intensity, float Rotation, float GradientAngle) {
		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source ? Source : TextureHelper::GetBlack());
		TiledBlobPtr CombinedMaskTexture = CombineIfNeeded(Cycle, TargetId, MaskTexture ? MaskTexture : TextureHelper::GetBlack());
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, CombinedSource);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBMultiDirectionalWarp>(Cycle, TargetId, TEXT("TGMB_MultiDirectionalWarp"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_BLOB(CombinedMaskTexture, "MaskTexture"))
			->AddArg(ARG_FLOAT(Intensity, "Intensity"))
			->AddArg(ARG_FLOAT(Rotation, "Rotation"))
			->AddArg(ARG_FLOAT(GradientAngle, "GradientAngle"))
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB MultiDirectionalWarp"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateNonUniformDirectionalWarp(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, TiledBlobPtr MaskTexture, float Intensity, float GradientAngle) {
		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source ? Source : TextureHelper::GetBlack());
		TiledBlobPtr CombinedMaskTexture = CombineIfNeeded(Cycle, TargetId, MaskTexture ? MaskTexture : TextureHelper::GetBlack());
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, CombinedSource);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBNonUniformDirectionalWarp>(Cycle, TargetId, TEXT("TGMB_NonUniformDirectionalWarp"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_BLOB(CombinedMaskTexture, "MaskTexture"))
			->AddArg(ARG_FLOAT(Intensity, "Intensity"))
			->AddArg(ARG_FLOAT(GradientAngle, "GradientAngle"))
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB NonUniformDirectionalWarp"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateDirectionalDistance(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, float Distance, float GradientAngle, int32 Samples, float Threshold) {
		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source ? Source : TextureHelper::GetBlack());
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, CombinedSource);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBDirectionalDistance>(Cycle, TargetId, TEXT("TGMB_DirectionalDistance"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_FLOAT(Distance, "Distance"))
			->AddArg(ARG_FLOAT(GradientAngle, "GradientAngle"))
			->AddArg(ARG_INT(Samples, "Samples"))
			->AddArg(ARG_FLOAT(Threshold, "Threshold"))
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB DirectionalDistance"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateShapeSplatter(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, TiledBlobPtr MaskTexture, float CountX, float CountY, float Scale, float Threshold) {
		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source ? Source : TextureHelper::GetBlack());
		TiledBlobPtr CombinedMaskTexture = CombineIfNeeded(Cycle, TargetId, MaskTexture ? MaskTexture : TextureHelper::GetBlack());
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, CombinedSource);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBShapeSplatter>(Cycle, TargetId, TEXT("TGMB_ShapeSplatter"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_BLOB(CombinedMaskTexture, "MaskTexture"))
			->AddArg(ARG_FLOAT(CountX, "CountX"))
			->AddArg(ARG_FLOAT(CountY, "CountY"))
			->AddArg(ARG_FLOAT(Scale, "Scale"))
			->AddArg(ARG_FLOAT(Threshold, "Threshold"))
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB ShapeSplatter"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateCells1(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, int32 Scale, float Disorder, float DisorderSpeed, float DisorderAnisotropy, float DisorderAnisotropyAngle, int32 PatternType, float PatternSizeX, float PatternSizeY, float PatternScale, float LuminanceRandom, float Angle, float AngleRandom, float OffsetX, float OffsetY, bool bNonSquareExpansion, int32 Seed) {
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, nullptr);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBCells1>(Cycle, TargetId, TEXT("TGMB_Cells1"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_FLOAT(static_cast<float>(FMath::Max(Scale, 1)), "Scale"))
			->AddArg(ARG_FLOAT(Disorder, "Disorder"))
			->AddArg(ARG_FLOAT(DisorderSpeed, "DisorderSpeed"))
			->AddArg(ARG_FLOAT(DisorderAnisotropy, "DisorderAnisotropy"))
			->AddArg(ARG_FLOAT(DisorderAnisotropyAngle, "DisorderAnisotropyAngle"))
			->AddArg(ARG_INT(FMath::Clamp(PatternType, 0, 3), "PatternType"))
			->AddArg(ARG_FLOAT(PatternSizeX, "PatternSizeX"))
			->AddArg(ARG_FLOAT(PatternSizeY, "PatternSizeY"))
			->AddArg(ARG_FLOAT(PatternScale, "PatternScale"))
			->AddArg(ARG_FLOAT(LuminanceRandom, "LuminanceRandom"))
			->AddArg(ARG_FLOAT(Angle, "Angle"))
			->AddArg(ARG_FLOAT(AngleRandom, "AngleRandom"))
			->AddArg(ARG_FLOAT(OffsetX, "OffsetX"))
			->AddArg(ARG_FLOAT(OffsetY, "OffsetY"))
			->AddArg(ARG_FLOAT(bNonSquareExpansion ? 1.0f : 0.0f, "NonSquareExpansion"))
			->AddArg(ARG_INT(Seed, "Seed"))
			->AddArg(WithUnbounded(ARG_INT(CellsRenderVersion, "CellsRenderVersion")))
			->AddArg(std::make_shared<JobArg_ForceTiling>())
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB Cells1"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateCells2(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, int32 Scale, float EdgeWidth, bool bInvert, float Disorder, float DisorderSpeed, float OffsetX, float OffsetY, bool bNonSquareExpansion, int32 Seed) {
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, nullptr);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBCells2>(Cycle, TargetId, TEXT("TGMB_Cells2"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_FLOAT(static_cast<float>(FMath::Max(Scale, 1)), "Scale"))
			->AddArg(ARG_FLOAT(EdgeWidth, "EdgeWidth"))
			->AddArg(ARG_FLOAT(bInvert ? 1.0f : 0.0f, "Invert"))
			->AddArg(ARG_FLOAT(Disorder, "Disorder"))
			->AddArg(ARG_FLOAT(DisorderSpeed, "DisorderSpeed"))
			->AddArg(ARG_FLOAT(OffsetX, "OffsetX"))
			->AddArg(ARG_FLOAT(OffsetY, "OffsetY"))
			->AddArg(ARG_FLOAT(bNonSquareExpansion ? 1.0f : 0.0f, "NonSquareExpansion"))
			->AddArg(ARG_INT(Seed, "Seed"))
			->AddArg(WithUnbounded(ARG_INT(CellsRenderVersion, "CellsRenderVersion")))
			->AddArg(std::make_shared<JobArg_ForceTiling>())
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB Cells2"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateCells3(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, int32 Scale, float Hardness, bool bInvert, float Disorder, float DisorderSpeed, float DisorderAnisotropy, float DisorderAnisotropyAngle, float PatternSizeX, float PatternSizeY, float PatternScale, float Angle, float AngleRandom, float OffsetX, float OffsetY, bool bNonSquareExpansion, int32 Seed) {
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, nullptr);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBCells3>(Cycle, TargetId, TEXT("TGMB_Cells3"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_FLOAT(static_cast<float>(FMath::Max(Scale, 1)), "Scale"))
			->AddArg(ARG_FLOAT(Hardness, "Hardness"))
			->AddArg(ARG_FLOAT(bInvert ? 1.0f : 0.0f, "Invert"))
			->AddArg(ARG_FLOAT(Disorder, "Disorder"))
			->AddArg(ARG_FLOAT(DisorderSpeed, "DisorderSpeed"))
			->AddArg(ARG_FLOAT(DisorderAnisotropy, "DisorderAnisotropy"))
			->AddArg(ARG_FLOAT(DisorderAnisotropyAngle, "DisorderAnisotropyAngle"))
			->AddArg(ARG_FLOAT(PatternSizeX, "PatternSizeX"))
			->AddArg(ARG_FLOAT(PatternSizeY, "PatternSizeY"))
			->AddArg(ARG_FLOAT(PatternScale, "PatternScale"))
			->AddArg(ARG_FLOAT(Angle, "Angle"))
			->AddArg(ARG_FLOAT(AngleRandom, "AngleRandom"))
			->AddArg(ARG_FLOAT(OffsetX, "OffsetX"))
			->AddArg(ARG_FLOAT(OffsetY, "OffsetY"))
			->AddArg(ARG_FLOAT(bNonSquareExpansion ? 1.0f : 0.0f, "NonSquareExpansion"))
			->AddArg(ARG_INT(Seed, "Seed"))
			->AddArg(WithUnbounded(ARG_INT(CellsRenderVersion, "CellsRenderVersion")))
			->AddArg(std::make_shared<JobArg_ForceTiling>())
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB Cells3"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateCells4(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, int32 Scale, float Disorder, float DisorderSpeed, int32 ColorSource, int32 PseudorandomSeed, float OffsetX, float OffsetY, bool bNonSquareExpansion, int32 Seed) {
		const bool bHasSource = Source != nullptr;
		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, bHasSource ? Source : TextureHelper::GetBlack());
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, bHasSource ? CombinedSource : nullptr);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBCells4>(Cycle, TargetId, TEXT("TGMB_Cells4"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_FLOAT(static_cast<float>(FMath::Max(Scale, 1)), "Scale"))
			->AddArg(ARG_FLOAT(Disorder, "Disorder"))
			->AddArg(ARG_FLOAT(DisorderSpeed, "DisorderSpeed"))
			->AddArg(ARG_INT(FMath::Clamp(ColorSource, 0, 2), "ColorSource"))
			->AddArg(ARG_INT(PseudorandomSeed, "PseudorandomSeed"))
			->AddArg(ARG_FLOAT(OffsetX, "OffsetX"))
			->AddArg(ARG_FLOAT(OffsetY, "OffsetY"))
			->AddArg(ARG_FLOAT(bNonSquareExpansion ? 1.0f : 0.0f, "NonSquareExpansion"))
			->AddArg(ARG_INT(Seed, "Seed"))
			->AddArg(WithUnbounded(ARG_INT(CellsRenderVersion, "CellsRenderVersion")))
			->AddArg(std::make_shared<JobArg_ForceTiling>())
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB Cells4"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateClouds2(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, float Scale, float Contrast, float Bias, float OffsetX, float OffsetY, int32 Seed) {
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, nullptr);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBClouds2>(Cycle, TargetId, TEXT("TGMB_Clouds2"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_FLOAT(Scale, "Scale"))
			->AddArg(ARG_FLOAT(Contrast, "Contrast"))
			->AddArg(ARG_FLOAT(Bias, "Bias"))
			->AddArg(ARG_FLOAT(OffsetX, "OffsetX"))
			->AddArg(ARG_FLOAT(OffsetY, "OffsetY"))
			->AddArg(ARG_INT(Seed, "Seed"))
			->AddArg(std::make_shared<JobArg_ForceTiling>())
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB Clouds2"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateBnWSpots(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, float Scale, float Threshold, float Smoothness, float OffsetX, float OffsetY, int32 Seed) {
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, nullptr);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBBnWSpots>(Cycle, TargetId, TEXT("TGMB_BnWSpots"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_FLOAT(Scale, "Scale"))
			->AddArg(ARG_FLOAT(Threshold, "Threshold"))
			->AddArg(ARG_FLOAT(Smoothness, "Smoothness"))
			->AddArg(ARG_FLOAT(OffsetX, "OffsetX"))
			->AddArg(ARG_FLOAT(OffsetY, "OffsetY"))
			->AddArg(ARG_INT(Seed, "Seed"))
			->AddArg(std::make_shared<JobArg_ForceTiling>())
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB BnWSpots"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateGrungeDirt(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, float Scale, float Contrast, float Bias, float OffsetX, float OffsetY, int32 Seed) {
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, nullptr);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBGrungeDirt>(Cycle, TargetId, TEXT("TGMB_GrungeDirt"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_FLOAT(Scale, "Scale"))
			->AddArg(ARG_FLOAT(Contrast, "Contrast"))
			->AddArg(ARG_FLOAT(Bias, "Bias"))
			->AddArg(ARG_FLOAT(OffsetX, "OffsetX"))
			->AddArg(ARG_FLOAT(OffsetY, "OffsetY"))
			->AddArg(ARG_INT(Seed, "Seed"))
			->AddArg(std::make_shared<JobArg_ForceTiling>())
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB GrungeDirt"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateHighpass(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, float Radius, float Contrast) {
		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source ? Source : TextureHelper::GetBlack());
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, CombinedSource);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBHighpass>(Cycle, TargetId, TEXT("TGMB_Highpass"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_FLOAT(Radius, "Radius"))
			->AddArg(ARG_FLOAT(Contrast, "Contrast"))
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB Highpass"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateLuminanceHighpass(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, float Radius, float Contrast) {
		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source ? Source : TextureHelper::GetBlack());
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, CombinedSource);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBLuminanceHighpass>(Cycle, TargetId, TEXT("TGMB_LuminanceHighpass"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_FLOAT(Radius, "Radius"))
			->AddArg(ARG_FLOAT(Contrast, "Contrast"))
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB LuminanceHighpass"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateBlurHQ(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, float Intensity, float Quality)
	{
		if (!Source)
		{
			return TextureHelper::GetBlack();
		}

		if (Intensity <= KINDA_SMALL_NUMBER)
		{
			return Source;
		}

		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source);
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, Source);
		FTileInfo TileInfo;

		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBBlurHQ>(Cycle, TargetId, TEXT("TGMB_BlurHQ"));
		RenderJob
			->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_FLOAT(FMath::Clamp(Intensity, 0.0f, 16.0f), "Intensity"))
			->AddArg(ARG_FLOAT(FMath::Clamp(Quality, 0.0f, 1.0f), "Quality"));

		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB Blur HQ"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
	}

	TiledBlobPtr FDesignerTransforms::CreateCurvatureSobel(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, float Radius, float Intensity) {
		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source ? Source : TextureHelper::GetBlack());
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, CombinedSource);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBCurvatureSobel>(Cycle, TargetId, TEXT("TGMB_CurvatureSobel"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_FLOAT(Radius, "Radius"))
			->AddArg(ARG_FLOAT(Intensity, "Intensity"))
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB CurvatureSobel"), &Desc);
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
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
