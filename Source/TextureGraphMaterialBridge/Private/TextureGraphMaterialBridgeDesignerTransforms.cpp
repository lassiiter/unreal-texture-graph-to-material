#if TGMB_WITH_DESIGNER_NODES

#include "TextureGraphMaterialBridgeDesignerTransforms.h"

#include "2D/TextureHelper.h"
#include "FxMat/MaterialManager.h"
#include "Job/JobArgs.h"
#include "Job/JobBatch.h"
#include "RHICommandList.h"
#include "TextureResource.h"
#include "TextureGraphEngine.h"
#include "Transform/Utility/T_CombineTiledBlob.h"

class CSH_TGMBFloodFillInit : public CmpSH_Base<16, 16, 1>
{
public:
	DECLARE_GLOBAL_SHADER(CSH_TGMBFloodFillInit);
	SHADER_USE_PARAMETER_STRUCT(CSH_TGMBFloodFillInit, CmpSH_Base);
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_UAV(RWTexture2D<float4>, Result)
		SHADER_PARAMETER_TEXTURE(Texture2D, SourceTexture)
		SHADER_PARAMETER(float, Threshold)
		SHADER_PARAMETER(int32, Connectivity)
		SHADER_PARAMETER(FIntVector4, Dimensions)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, InitLabels)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, InitBounds)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};

class CSH_TGMBFloodFillUnion : public CmpSH_Base<16, 16, 1>
{
public:
	DECLARE_GLOBAL_SHADER(CSH_TGMBFloodFillUnion);
	SHADER_USE_PARAMETER_STRUCT(CSH_TGMBFloodFillUnion, CmpSH_Base);
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(int32, Connectivity)
		SHADER_PARAMETER(FIntVector4, Dimensions)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, UnionLabels)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};

class CSH_TGMBFloodFillCompress : public CmpSH_Base<16, 16, 1>
{
public:
	DECLARE_GLOBAL_SHADER(CSH_TGMBFloodFillCompress);
	SHADER_USE_PARAMETER_STRUCT(CSH_TGMBFloodFillCompress, CmpSH_Base);
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(FIntVector4, Dimensions)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, CompressLabels)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};

class CSH_TGMBFloodFillResetBounds : public CmpSH_Base<16, 16, 1>
{
public:
	DECLARE_GLOBAL_SHADER(CSH_TGMBFloodFillResetBounds);
	SHADER_USE_PARAMETER_STRUCT(CSH_TGMBFloodFillResetBounds, CmpSH_Base);
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(FIntVector4, Dimensions)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, ResetBounds)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};

class CSH_TGMBFloodFillReduceBounds : public CmpSH_Base<16, 16, 1>
{
public:
	DECLARE_GLOBAL_SHADER(CSH_TGMBFloodFillReduceBounds);
	SHADER_USE_PARAMETER_STRUCT(CSH_TGMBFloodFillReduceBounds, CmpSH_Base);
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(FIntVector4, Dimensions)
		SHADER_PARAMETER_SRV(StructuredBuffer<uint>, Labels)
		SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, ReduceBounds)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};

class CSH_TGMBFloodFillEmitData : public CmpSH_Base<16, 16, 1>
{
public:
	DECLARE_GLOBAL_SHADER(CSH_TGMBFloodFillEmitData);
	SHADER_USE_PARAMETER_STRUCT(CSH_TGMBFloodFillEmitData, CmpSH_Base);
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(FIntVector4, Dimensions)
		SHADER_PARAMETER_SRV(StructuredBuffer<uint>, FinalLabels)
		SHADER_PARAMETER_SRV(StructuredBuffer<uint>, FinalBounds)
		SHADER_PARAMETER_UAV(RWTexture2D<float4>, Result)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};

IMPLEMENT_GLOBAL_SHADER(CSH_TGMBFloodFillInit, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_FloodFillCompute.usf", "CSH_TGMBFloodFillInit", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(CSH_TGMBFloodFillUnion, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_FloodFillCompute.usf", "CSH_TGMBFloodFillUnion", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(CSH_TGMBFloodFillCompress, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_FloodFillCompute.usf", "CSH_TGMBFloodFillCompress", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(CSH_TGMBFloodFillResetBounds, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_FloodFillCompute.usf", "CSH_TGMBFloodFillResetBounds", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(CSH_TGMBFloodFillReduceBounds, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_FloodFillCompute.usf", "CSH_TGMBFloodFillReduceBounds", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(CSH_TGMBFloodFillEmitData, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_FloodFillCompute.usf", "CSH_TGMBFloodFillEmitData", SF_Compute);

IMPLEMENT_GLOBAL_SHADER(FSH_TGMBFloodFillToRandomGrayscale, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBFloodFillToRandomGrayscale", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBFloodFillToRandomColor, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBFloodFillToRandomColor", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBFloodFillToGradient, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBFloodFillToGradient", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBFloodFillToPosition, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBFloodFillToPosition", SF_Pixel);
IMPLEMENT_GLOBAL_SHADER(FSH_TGMBFloodFillToBBoxSize, "/Plugin/TextureGraphMaterialBridge/Expressions/TGMB_DesignerNodes.usf", "FSH_TGMBFloodFillToBBoxSize", SF_Pixel);
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

		BufferDescriptor BuildFloodFillDataDesc(BufferDescriptor DesiredDesc, TiledBlobPtr Reference)
		{
			BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, Reference, 4);
			Desc.Format = BufferFormat::Float;
			Desc.ItemsPerPoint = 4;
			Desc.DefaultValue = FLinearColor::Black;
			Desc.AddMetadata(TEXT("FX:UAV"));
			return Desc;
		}

		int32 FloodFillResolvePassCount(int32 Width, int32 Height)
		{
			const uint64 PixelCount = static_cast<uint64>(FMath::Max(Width, 1)) * static_cast<uint64>(FMath::Max(Height, 1));
			uint64 PowerOfTwo = 1;
			int32 LogPixels = 0;
			while (PowerOfTwo < PixelCount && LogPixels < 63)
			{
				PowerOfTwo <<= 1;
				++LogPixels;
			}

			return FMath::Clamp(LogPixels * 4 + 4, 16, 128);
		}

		template <typename ShaderType, typename ParametersType>
		void DispatchFloodFillShader(FRHICommandListImmediate& RHI, const ParametersType& Parameters, int32 Width, int32 Height)
		{
			TShaderMapRef<ShaderType> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
			const FIntVector GroupSize = ComputeShader->ThreadGroupSize();
			FComputeShaderUtils::Dispatch(
				RHI,
				ComputeShader,
				Parameters,
				FIntVector(
					FMath::DivideAndRoundUp(Width, GroupSize.X),
					FMath::DivideAndRoundUp(Height, GroupSize.Y),
					1));
		}

		class FxMaterial_TGMBFloodFillData : public FxMaterial_Compute<CSH_TGMBFloodFillInit>
		{
			FRWBufferStructured LabelsBuffer;
			FRWBufferStructured BoundsBuffer;

		public:
			using CmpSHPermutationDomain = typename CSH_TGMBFloodFillInit::FPermutationDomain;

			FxMaterial_TGMBFloodFillData(
				FString InOutputId,
				const CmpSHPermutationDomain* InPermutationDomain = nullptr,
				int InNumThreadsX = FxMaterial_Compute<CSH_TGMBFloodFillInit>::GDefaultNumThreadsXY,
				int InNumThreadsY = FxMaterial_Compute<CSH_TGMBFloodFillInit>::GDefaultNumThreadsXY,
				int InNumThreadsZ = 1,
				FUnorderedAccessViewRHIRef InUnorderedAccessView = nullptr)
				: FxMaterial_Compute<CSH_TGMBFloodFillInit>(InOutputId, InPermutationDomain, InNumThreadsX, InNumThreadsY, InNumThreadsZ, InUnorderedAccessView)
			{
			}

			virtual std::shared_ptr<FxMaterial> Clone() override
			{
				return std::static_pointer_cast<FxMaterial>(std::make_shared<FxMaterial_TGMBFloodFillData>(
					OutputId,
					&PermutationDomain,
					NumThreadsX,
					NumThreadsY,
					NumThreadsZ,
					UnorderedAccessView));
			}

			virtual void Blit(FRHICommandListImmediate& RHI, FRHITexture* Target, const RenderMesh* MeshObj, int32 TargetId, FGraphicsPipelineStateInitializer* PSO = nullptr) override
			{
				BindTexturesForBlitting();

				QUICK_SCOPE_CYCLE_COUNTER(STAT_TGMB_FloodFillData);
				SCOPED_DRAW_EVENT(RHI, TGMB_FloodFillData);

				const FIntPoint TargetSize = Target->GetSizeXY();
				const int32 Width = FMath::Max(TargetSize.X, 1);
				const int32 Height = FMath::Max(TargetSize.Y, 1);
				const uint32 PixelCount = static_cast<uint32>(Width * Height);

				LabelsBuffer.Release();
				BoundsBuffer.Release();
				LabelsBuffer.Initialize(RHI, TEXT("TGMBFloodFillLabels"), sizeof(uint32), PixelCount);
				BoundsBuffer.Initialize(RHI, TEXT("TGMBFloodFillBounds"), sizeof(uint32), PixelCount * 4);

				FUnorderedAccessViewRHIRef RenderTargetUAV = UnorderedAccessView;
				if (!RenderTargetUAV)
				{
					RenderTargetUAV = RHI.CreateUnorderedAccessView(Target, FRHIViewDesc::CreateTextureUAV().SetDimensionFromTexture(Target));
				}

				const FIntVector4 PassDimensions(Width, Height, 0, 0);

				CSH_TGMBFloodFillInit::FParameters InitParams = Params;
				InitParams.Dimensions = PassDimensions;
				InitParams.Result = RenderTargetUAV;
				InitParams.InitLabels = LabelsBuffer.UAV;
				InitParams.InitBounds = BoundsBuffer.UAV;

				RHI.Transition(FRHITransitionInfo(Target, ERHIAccess::Unknown, ERHIAccess::UAVMask));
				DispatchFloodFillShader<CSH_TGMBFloodFillInit>(RHI, InitParams, Width, Height);
				RHI.Transition(FRHITransitionInfo(LabelsBuffer.UAV, ERHIAccess::UAVMask, ERHIAccess::UAVMask));
				RHI.Transition(FRHITransitionInfo(BoundsBuffer.UAV, ERHIAccess::UAVMask, ERHIAccess::UAVMask));

				CSH_TGMBFloodFillUnion::FParameters UnionParams;
				UnionParams.Connectivity = InitParams.Connectivity;
				UnionParams.Dimensions = PassDimensions;
				UnionParams.UnionLabels = LabelsBuffer.UAV;

				CSH_TGMBFloodFillCompress::FParameters CompressParams;
				CompressParams.Dimensions = PassDimensions;
				CompressParams.CompressLabels = LabelsBuffer.UAV;

				const int32 ResolvePasses = FloodFillResolvePassCount(Width, Height);
				for (int32 PassIndex = 0; PassIndex < ResolvePasses; ++PassIndex)
				{
					DispatchFloodFillShader<CSH_TGMBFloodFillUnion>(RHI, UnionParams, Width, Height);
					RHI.Transition(FRHITransitionInfo(LabelsBuffer.UAV, ERHIAccess::UAVMask, ERHIAccess::UAVMask));

					DispatchFloodFillShader<CSH_TGMBFloodFillCompress>(RHI, CompressParams, Width, Height);
					RHI.Transition(FRHITransitionInfo(LabelsBuffer.UAV, ERHIAccess::UAVMask, ERHIAccess::UAVMask));
				}

				CSH_TGMBFloodFillResetBounds::FParameters ResetBoundsParams;
				ResetBoundsParams.Dimensions = PassDimensions;
				ResetBoundsParams.ResetBounds = BoundsBuffer.UAV;
				DispatchFloodFillShader<CSH_TGMBFloodFillResetBounds>(RHI, ResetBoundsParams, Width, Height);
				RHI.Transition(FRHITransitionInfo(BoundsBuffer.UAV, ERHIAccess::UAVMask, ERHIAccess::UAVMask));

				RHI.Transition(FRHITransitionInfo(LabelsBuffer.UAV, ERHIAccess::UAVMask, ERHIAccess::SRVMask));

				CSH_TGMBFloodFillReduceBounds::FParameters ReduceBoundsParams;
				ReduceBoundsParams.Dimensions = PassDimensions;
				ReduceBoundsParams.Labels = LabelsBuffer.SRV;
				ReduceBoundsParams.ReduceBounds = BoundsBuffer.UAV;
				DispatchFloodFillShader<CSH_TGMBFloodFillReduceBounds>(RHI, ReduceBoundsParams, Width, Height);
				RHI.Transition(FRHITransitionInfo(BoundsBuffer.UAV, ERHIAccess::UAVMask, ERHIAccess::SRVMask));

				CSH_TGMBFloodFillEmitData::FParameters EmitDataParams;
				EmitDataParams.Dimensions = PassDimensions;
				EmitDataParams.FinalLabels = LabelsBuffer.SRV;
				EmitDataParams.FinalBounds = BoundsBuffer.SRV;
				EmitDataParams.Result = RenderTargetUAV;
				DispatchFloodFillShader<CSH_TGMBFloodFillEmitData>(RHI, EmitDataParams, Width, Height);

				RHI.Transition(FRHITransitionInfo(Target, ERHIAccess::UAVMask, ERHIAccess::SRVMask));
			}
		};

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

	
	TiledBlobPtr FDesignerTransforms::CreateFloodFillData(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, int32 Connectivity, float Threshold)
	{
		if (!Source)
		{
			return TextureHelper::GetBlack();
		}

		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source);
		BufferDescriptor Desc = BuildFloodFillDataDesc(DesiredDesc, Source);

		CSH_TGMBFloodFillInit::FPermutationDomain PermutationVector;
		std::shared_ptr<FxMaterial_TGMBFloodFillData> FxMat = std::make_shared<FxMaterial_TGMBFloodFillData>(TEXT("Result"), &PermutationVector, Desc.Width, Desc.Height, 1);
		RenderMaterial_FXPtr Transform = std::make_shared<RenderMaterial_FX>(TEXT("TGMB_FloodFillData"), std::static_pointer_cast<FxMaterial>(FxMat));
		JobUPtr RenderJob = std::make_unique<Job>(Cycle->GetMix(), TargetId, std::static_pointer_cast<BlobTransform>(Transform));
		RenderJob
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_FLOAT(Threshold, "Threshold"))
			->AddArg(ARG_INT(Connectivity, "Connectivity"));

		RenderJob->SetTiled(false);
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB Flood Fill Data"), &Desc, 1, 1);
		Result->MakeSingleBlob();
		Cycle->AddJob(TargetId, std::move(RenderJob));
		return Result;
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

	TiledBlobPtr FDesignerTransforms::CreateFloodFillToBBoxSize(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, int32 OutputMode) {
		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source ? Source : TextureHelper::GetBlack());
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, CombinedSource);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBFloodFillToBBoxSize>(Cycle, TargetId, TEXT("TGMB_FloodFillToBBoxSize"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_INT(OutputMode, "OutputMode"))
			;
		TiledBlobPtr Result = RenderJob->InitResult(TEXT("TGMB FloodFillToBBoxSize"), &Desc);
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

	TiledBlobPtr FDesignerTransforms::CreateCurvatureSobel(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, int32 OutputMode, float Radius, float Intensity, float Threshold) {
		TiledBlobPtr CombinedSource = CombineIfNeeded(Cycle, TargetId, Source ? Source : TextureHelper::GetBlack());
		BufferDescriptor Desc = BuildOutputDesc(DesiredDesc, CombinedSource);
		FTileInfo TileInfo;
		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBCurvatureSobel>(Cycle, TargetId, TEXT("TGMB_CurvatureSobel"));
		RenderJob->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
			->AddArg(ARG_BLOB(CombinedSource, "SourceTexture"))
			->AddArg(ARG_INT(FMath::Clamp(OutputMode, 0, 1), "OutputMode"))
			->AddArg(ARG_FLOAT(FMath::Clamp(Radius, 0.0f, 64.0f), "Radius"))
			->AddArg(ARG_FLOAT(FMath::Clamp(Intensity, 0.0f, 64.0f), "Intensity"))
			->AddArg(ARG_FLOAT(FMath::Clamp(Threshold, 0.0f, 1.0f), "Threshold"))
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
		FTileInfo TileInfo;

		JobUPtr RenderJob = CreateShaderJob<FSH_TGMBGradientMap>(Cycle, TargetId, TEXT("TGMB_GradientMap"));
		RenderJob
			->AddArg(ARG_TILEINFO(TileInfo, "TileInfo"))
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

	TiledBlobPtr FDesignerTransforms::CreateFloodFill(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, int32 OutputMode, int32 Seed, float GradientAngle)
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
			->AddArg(ARG_INT(Seed, "Seed"))
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
			->AddArg(ARG_INT(FMath::Clamp(Samples, 1, 256), "Samples"))
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

#endif
