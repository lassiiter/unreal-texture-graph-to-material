#pragma once

#include "CoreMinimal.h"
#include "DataDrivenShaderPlatformInfo.h"
#include "FxMat/FxMaterial.h"
#include "Job/Job.h"
#include "Model/Mix/MixUpdateCycle.h"

#define UE_API TEXTUREGRAPHMATERIALBRIDGE_API

class FSH_TGMBTileGenerator : public FSH_Base
{
public:
	DECLARE_EXPORTED_GLOBAL_SHADER(FSH_TGMBTileGenerator, UE_API);
	SHADER_USE_PARAMETER_STRUCT(FSH_TGMBTileGenerator, FSH_Base);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FTileInfo, TileInfo)
		SHADER_PARAMETER_STRUCT(FStandardSamplerStates, SamplerStates)
		SHADER_PARAMETER(int32, OutputMode)
		SHADER_PARAMETER(int32, PatternType)
		SHADER_PARAMETER(int32, Seed)
		SHADER_PARAMETER(float, CountX)
		SHADER_PARAMETER(float, CountY)
		SHADER_PARAMETER(float, Scale)
		SHADER_PARAMETER(float, Spacing)
		SHADER_PARAMETER(float, OffsetX)
		SHADER_PARAMETER(float, OffsetY)
		SHADER_PARAMETER(float, Rotation)
		SHADER_PARAMETER(float, PositionJitter)
		SHADER_PARAMETER(float, SizeJitter)
		SHADER_PARAMETER(float, RotationJitter)
		SHADER_PARAMETER(float, LuminanceJitter)
	END_SHADER_PARAMETER_STRUCT()

	TEXTURE_ENGINE_DEFAULT_PERMUTATION;
	TEXTUREGRAPH_ENGINE_DEFAULT_COMPILATION_ENV;
};

class FSH_TGMBTileSampler : public FSH_Base
{
public:
	DECLARE_EXPORTED_GLOBAL_SHADER(FSH_TGMBTileSampler, UE_API);
	SHADER_USE_PARAMETER_STRUCT(FSH_TGMBTileSampler, FSH_Base);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FTileInfo, TileInfo)
		SHADER_PARAMETER_STRUCT(FStandardSamplerStates, SamplerStates)
		SHADER_PARAMETER_TEXTURE(Texture2D, SourceTexture)
		SHADER_PARAMETER_TEXTURE(Texture2D, MaskTexture)
		SHADER_PARAMETER_TEXTURE(Texture2D, ScaleMap)
		SHADER_PARAMETER_TEXTURE(Texture2D, RotationMap)
		SHADER_PARAMETER(int32, Seed)
		SHADER_PARAMETER(float, CountX)
		SHADER_PARAMETER(float, CountY)
		SHADER_PARAMETER(float, Scale)
		SHADER_PARAMETER(float, Spacing)
		SHADER_PARAMETER(float, Rotation)
		SHADER_PARAMETER(float, PositionJitter)
		SHADER_PARAMETER(float, SizeJitter)
		SHADER_PARAMETER(float, RotationJitter)
	END_SHADER_PARAMETER_STRUCT()

	TEXTURE_ENGINE_DEFAULT_PERMUTATION;
	TEXTUREGRAPH_ENGINE_DEFAULT_COMPILATION_ENV;
};

class FSH_TGMBGradientMap : public FSH_Base
{
public:
	DECLARE_EXPORTED_GLOBAL_SHADER(FSH_TGMBGradientMap, UE_API);
	SHADER_USE_PARAMETER_STRUCT(FSH_TGMBGradientMap, FSH_Base);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FStandardSamplerStates, SamplerStates)
		SHADER_PARAMETER_TEXTURE(Texture2D, SourceTexture)
		SHADER_PARAMETER(int32, NumStops)
		SHADER_PARAMETER(int32, Interpolation)
		SHADER_PARAMETER(int32, AddressMode)
		SHADER_PARAMETER(float, Position0)
		SHADER_PARAMETER(float, Position1)
		SHADER_PARAMETER(float, Position2)
		SHADER_PARAMETER(float, Position3)
		SHADER_PARAMETER(float, Position4)
		SHADER_PARAMETER(float, Position5)
		SHADER_PARAMETER(float, Position6)
		SHADER_PARAMETER(float, Position7)
		SHADER_PARAMETER(FLinearColor, Color0)
		SHADER_PARAMETER(FLinearColor, Color1)
		SHADER_PARAMETER(FLinearColor, Color2)
		SHADER_PARAMETER(FLinearColor, Color3)
		SHADER_PARAMETER(FLinearColor, Color4)
		SHADER_PARAMETER(FLinearColor, Color5)
		SHADER_PARAMETER(FLinearColor, Color6)
		SHADER_PARAMETER(FLinearColor, Color7)
	END_SHADER_PARAMETER_STRUCT()

	TEXTURE_ENGINE_DEFAULT_PERMUTATION;
	TEXTUREGRAPH_ENGINE_DEFAULT_COMPILATION_ENV;
};

class FSH_TGMBNormalCombine : public FSH_Base
{
public:
	DECLARE_EXPORTED_GLOBAL_SHADER(FSH_TGMBNormalCombine, UE_API);
	SHADER_USE_PARAMETER_STRUCT(FSH_TGMBNormalCombine, FSH_Base);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FStandardSamplerStates, SamplerStates)
		SHADER_PARAMETER_TEXTURE(Texture2D, BaseNormal)
		SHADER_PARAMETER_TEXTURE(Texture2D, DetailNormal)
		SHADER_PARAMETER_TEXTURE(Texture2D, MaskTexture)
		SHADER_PARAMETER(int32, BlendMode)
		SHADER_PARAMETER(float, DetailStrength)
		SHADER_PARAMETER(float, InvertGreen)
	END_SHADER_PARAMETER_STRUCT()

	TEXTURE_ENGINE_DEFAULT_PERMUTATION;
	TEXTUREGRAPH_ENGINE_DEFAULT_COMPILATION_ENV;
};

class FSH_TGMBFloodFill : public FSH_Base
{
public:
	DECLARE_EXPORTED_GLOBAL_SHADER(FSH_TGMBFloodFill, UE_API);
	SHADER_USE_PARAMETER_STRUCT(FSH_TGMBFloodFill, FSH_Base);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FTileInfo, TileInfo)
		SHADER_PARAMETER_STRUCT(FStandardSamplerStates, SamplerStates)
		SHADER_PARAMETER_TEXTURE(Texture2D, SourceTexture)
		SHADER_PARAMETER(int32, OutputMode)
		SHADER_PARAMETER(int32, Connectivity)
		SHADER_PARAMETER(int32, Seed)
		SHADER_PARAMETER(float, Threshold)
		SHADER_PARAMETER(float, GradientAngle)
	END_SHADER_PARAMETER_STRUCT()

	TEXTURE_ENGINE_DEFAULT_PERMUTATION;
	TEXTUREGRAPH_ENGINE_DEFAULT_COMPILATION_ENV;
};

class FSH_TGMBSlopeBlur : public FSH_Base
{
public:
	DECLARE_EXPORTED_GLOBAL_SHADER(FSH_TGMBSlopeBlur, UE_API);
	SHADER_USE_PARAMETER_STRUCT(FSH_TGMBSlopeBlur, FSH_Base);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FTileInfo, TileInfo)
		SHADER_PARAMETER_STRUCT(FStandardSamplerStates, SamplerStates)
		SHADER_PARAMETER_TEXTURE(Texture2D, SourceTexture)
		SHADER_PARAMETER_TEXTURE(Texture2D, SlopeTexture)
		SHADER_PARAMETER(int32, Samples)
		SHADER_PARAMETER(int32, Mode)
		SHADER_PARAMETER(float, Intensity)
		SHADER_PARAMETER(float, InvertSlope)
	END_SHADER_PARAMETER_STRUCT()

	TEXTURE_ENGINE_DEFAULT_PERMUTATION;
	TEXTUREGRAPH_ENGINE_DEFAULT_COMPILATION_ENV;
};

class FSH_TGMBBevel : public FSH_Base
{
public:
	DECLARE_EXPORTED_GLOBAL_SHADER(FSH_TGMBBevel, UE_API);
	SHADER_USE_PARAMETER_STRUCT(FSH_TGMBBevel, FSH_Base);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FTileInfo, TileInfo)
		SHADER_PARAMETER_STRUCT(FStandardSamplerStates, SamplerStates)
		SHADER_PARAMETER_TEXTURE(Texture2D, SourceTexture)
		SHADER_PARAMETER(int32, Mode)
		SHADER_PARAMETER(float, Distance)
		SHADER_PARAMETER(float, Smoothness)
		SHADER_PARAMETER(float, Threshold)
	END_SHADER_PARAMETER_STRUCT()

	TEXTURE_ENGINE_DEFAULT_PERMUTATION;
	TEXTUREGRAPH_ENGINE_DEFAULT_COMPILATION_ENV;
};

class FSH_TGMBHBAO : public FSH_Base
{
public:
	DECLARE_EXPORTED_GLOBAL_SHADER(FSH_TGMBHBAO, UE_API);
	SHADER_USE_PARAMETER_STRUCT(FSH_TGMBHBAO, FSH_Base);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FTileInfo, TileInfo)
		SHADER_PARAMETER_STRUCT(FStandardSamplerStates, SamplerStates)
		SHADER_PARAMETER_TEXTURE(Texture2D, SourceTexture)
		SHADER_PARAMETER(int32, Samples)
		SHADER_PARAMETER(float, Radius)
		SHADER_PARAMETER(float, HeightScale)
		SHADER_PARAMETER(float, Bias)
		SHADER_PARAMETER(float, Contrast)
		SHADER_PARAMETER(float, Invert)
	END_SHADER_PARAMETER_STRUCT()

	TEXTURE_ENGINE_DEFAULT_PERMUTATION;
	TEXTUREGRAPH_ENGINE_DEFAULT_COMPILATION_ENV;
};

class FSH_TGMBCurvatureSmooth : public FSH_Base
{
public:
	DECLARE_EXPORTED_GLOBAL_SHADER(FSH_TGMBCurvatureSmooth, UE_API);
	SHADER_USE_PARAMETER_STRUCT(FSH_TGMBCurvatureSmooth, FSH_Base);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FTileInfo, TileInfo)
		SHADER_PARAMETER_STRUCT(FStandardSamplerStates, SamplerStates)
		SHADER_PARAMETER_TEXTURE(Texture2D, SourceTexture)
		SHADER_PARAMETER(int32, OutputMode)
		SHADER_PARAMETER(float, Radius)
		SHADER_PARAMETER(float, Smoothing)
		SHADER_PARAMETER(float, Intensity)
	END_SHADER_PARAMETER_STRUCT()

	TEXTURE_ENGINE_DEFAULT_PERMUTATION;
	TEXTUREGRAPH_ENGINE_DEFAULT_COMPILATION_ENV;
};

class FSH_TGMBMultiMaterialBlend : public FSH_Base
{
public:
	DECLARE_EXPORTED_GLOBAL_SHADER(FSH_TGMBMultiMaterialBlend, UE_API);
	SHADER_USE_PARAMETER_STRUCT(FSH_TGMBMultiMaterialBlend, FSH_Base);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT(FStandardSamplerStates, SamplerStates)
		SHADER_PARAMETER_TEXTURE(Texture2D, ABaseColor)
		SHADER_PARAMETER_TEXTURE(Texture2D, ANormal)
		SHADER_PARAMETER_TEXTURE(Texture2D, AORM)
		SHADER_PARAMETER_TEXTURE(Texture2D, AHeight)
		SHADER_PARAMETER_TEXTURE(Texture2D, BBaseColor)
		SHADER_PARAMETER_TEXTURE(Texture2D, BNormal)
		SHADER_PARAMETER_TEXTURE(Texture2D, BORM)
		SHADER_PARAMETER_TEXTURE(Texture2D, BHeight)
		SHADER_PARAMETER_TEXTURE(Texture2D, MaskTexture)
		SHADER_PARAMETER(int32, OutputMode)
		SHADER_PARAMETER(float, NormalStrength)
	END_SHADER_PARAMETER_STRUCT()

	TEXTURE_ENGINE_DEFAULT_PERMUTATION;
	TEXTUREGRAPH_ENGINE_DEFAULT_COMPILATION_ENV;
};

namespace UE::TextureGraphMaterialBridge
{
	struct FGradientMapSettings
	{
		int32 NumStops = 2;
		int32 Interpolation = 1;
		int32 AddressMode = 0;
		float Positions[8] = { 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
		FLinearColor Colors[8] =
		{
			FLinearColor::Black,
			FLinearColor::White,
			FLinearColor::White,
			FLinearColor::White,
			FLinearColor::White,
			FLinearColor::White,
			FLinearColor::White,
			FLinearColor::White
		};
	};

	class FDesignerTransforms
	{
	public:
		static TiledBlobPtr CreateTileGenerator(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, int32 OutputMode, int32 PatternType, int32 Seed, float CountX, float CountY, float Scale, float Spacing, float OffsetX, float OffsetY, float Rotation, float PositionJitter, float SizeJitter, float RotationJitter, float LuminanceJitter);
		static TiledBlobPtr CreateTileSampler(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, TiledBlobPtr Mask, TiledBlobPtr ScaleMap, TiledBlobPtr RotationMap, int32 Seed, float CountX, float CountY, float Scale, float Spacing, float Rotation, float PositionJitter, float SizeJitter, float RotationJitter);
		static TiledBlobPtr CreateGradientMap(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, const FGradientMapSettings& Settings);
		static TiledBlobPtr CreateNormalCombine(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr BaseNormal, TiledBlobPtr DetailNormal, TiledBlobPtr Mask, int32 BlendMode, float DetailStrength, bool bInvertGreen);
		static TiledBlobPtr CreateFloodFill(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, int32 OutputMode, int32 Connectivity, int32 Seed, float Threshold, float GradientAngle);
		static TiledBlobPtr CreateSlopeBlur(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, TiledBlobPtr Slope, float Intensity, int32 Samples, int32 Mode, bool bInvertSlope);
		static TiledBlobPtr CreateBevel(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, float Distance, float Smoothness, int32 Mode, float Threshold);
		static TiledBlobPtr CreateHBAO(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, float Radius, int32 Samples, float HeightScale, float Bias, float Contrast, bool bInvert);
		static TiledBlobPtr CreateCurvatureSmooth(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr Source, int32 OutputMode, float Radius, float Smoothing, float Intensity);
		static TiledBlobPtr CreateMultiMaterialBlend(MixUpdateCyclePtr Cycle, BufferDescriptor DesiredDesc, int32 TargetId, TiledBlobPtr ABaseColor, TiledBlobPtr ANormal, TiledBlobPtr AORM, TiledBlobPtr AHeight, TiledBlobPtr BBaseColor, TiledBlobPtr BNormal, TiledBlobPtr BORM, TiledBlobPtr BHeight, TiledBlobPtr Mask, int32 OutputMode, float NormalStrength);
	};
}

#undef UE_API
