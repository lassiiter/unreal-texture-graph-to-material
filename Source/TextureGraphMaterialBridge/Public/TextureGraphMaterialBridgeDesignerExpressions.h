#pragma once

#include "CoreMinimal.h"
#include "Expressions/TG_Expression.h"
#include "TG_Texture.h"
#include "TextureGraphMaterialBridgeDesignerExpressions.generated.h"

UENUM(BlueprintType)
enum class ETGMBTilePattern : uint8
{
	Square = 0 UMETA(DisplayName = "Square"),
	Circle = 1 UMETA(DisplayName = "Circle"),
	Diamond = 2 UMETA(DisplayName = "Diamond"),
	Checker = 3 UMETA(DisplayName = "Checker")
};

UENUM(BlueprintType)
enum class ETGMBTileGeneratorOutput : uint8
{
	Mask = 0 UMETA(DisplayName = "Mask"),
	ID = 1 UMETA(DisplayName = "ID"),
	RandomColor = 2 UMETA(DisplayName = "Random Color")
};

UENUM(BlueprintType)
enum class ETGMBGradientMapInterpolation : uint8
{
	Constant = 0 UMETA(DisplayName = "Constant"),
	Linear = 1 UMETA(DisplayName = "Linear"),
	Smooth = 2 UMETA(DisplayName = "Smooth")
};

UENUM(BlueprintType)
enum class ETGMBGradientMapAddressMode : uint8
{
	Clamp = 0 UMETA(DisplayName = "Clamp"),
	Repeat = 1 UMETA(DisplayName = "Repeat")
};

UENUM(BlueprintType)
enum class ETGMBNormalCombineMode : uint8
{
	RNM = 0 UMETA(DisplayName = "Reoriented Normal Mapping"),
	Whiteout = 1 UMETA(DisplayName = "Whiteout"),
	UDN = 2 UMETA(DisplayName = "UDN")
};

UENUM(BlueprintType)
enum class ETGMBFloodFillOutput : uint8
{
	ID = 0 UMETA(DisplayName = "ID"),
	RandomColor = 1 UMETA(DisplayName = "Random Color"),
	Position = 2 UMETA(DisplayName = "Position"),
	Gradient = 3 UMETA(DisplayName = "Gradient")
};

UENUM(BlueprintType)
enum class ETGMBFloodFillConnectivity : uint8
{
	FourWay = 0 UMETA(DisplayName = "4-Way"),
	EightWay = 1 UMETA(DisplayName = "8-Way")
};

UENUM(BlueprintType)
enum class ETGMBSlopeBlurMode : uint8
{
	Blur = 0 UMETA(DisplayName = "Blur"),
	Min = 1 UMETA(DisplayName = "Min"),
	Max = 2 UMETA(DisplayName = "Max")
};

UENUM(BlueprintType)
enum class ETGMBBevelMode : uint8
{
	Inside = 0 UMETA(DisplayName = "Inside"),
	Outside = 1 UMETA(DisplayName = "Outside"),
	Both = 2 UMETA(DisplayName = "Both")
};

UENUM(BlueprintType)
enum class ETGMBCurvatureSobelOutput : uint8
{
	Strength = 0 UMETA(DisplayName = "Strength"),
	BinaryMask = 1 UMETA(DisplayName = "Binary Mask")
};

UENUM(BlueprintType)
enum class ETGMBCellsPattern : uint8
{
	Disc = 0 UMETA(DisplayName = "Disc"),
	Square = 1 UMETA(DisplayName = "Square"),
	Diamond = 2 UMETA(DisplayName = "Diamond"),
	Bar = 3 UMETA(DisplayName = "Bar")
};

UENUM(BlueprintType)
enum class ETGMBCells4ColorSource : uint8
{
	Random = 0 UMETA(DisplayName = "Random"),
	Pseudorandom = 1 UMETA(DisplayName = "Pseudorandom"),
	ImageInput = 2 UMETA(DisplayName = "Image Input")
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_FloodFillToRandomGrayscale : public UTG_Expression {
	GENERATED_BODY()
public:
	TG_DECLARE_EXPRESSION(TG_Category::Filter);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Flood Fill to Random Grayscale"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Converts flood fill data to random grayscale.")); }
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Flood Fill Data")) FTG_Texture Source;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) int32 Seed = 0;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_FloodFillToRandomColor : public UTG_Expression {
	GENERATED_BODY()
public:
	TG_DECLARE_EXPRESSION(TG_Category::Filter);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Flood Fill to Random Color"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Converts flood fill data to random color.")); }
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Flood Fill Data")) FTG_Texture Source;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) int32 Seed = 0;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_FloodFillToGradient : public UTG_Expression {
	GENERATED_BODY()
public:
	TG_DECLARE_EXPRESSION(TG_Category::Filter);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Flood Fill to Gradient"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Converts flood fill data to gradient.")); }
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Flood Fill Data")) FTG_Texture Source;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float GradientAngle = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float RotationJitter = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) int32 Seed = 0;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_FloodFillToPosition : public UTG_Expression {
	GENERATED_BODY()
public:
	TG_DECLARE_EXPRESSION(TG_Category::Filter);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Flood Fill to Position"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Converts flood fill data to position.")); }
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Flood Fill Data")) FTG_Texture Source;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_FloodFillMapper : public UTG_Expression {
	GENERATED_BODY()
public:
	TG_DECLARE_EXPRESSION(TG_Category::Filter);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Flood Fill Mapper"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Maps a texture into flood fill cells.")); }
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Flood Fill Data")) FTG_Texture Source;
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Map")) FTG_Texture MaskTexture;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_MultiDirectionalWarp : public UTG_Expression {
	GENERATED_BODY()
public:
	TG_DECLARE_EXPRESSION(TG_Category::Filter);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Multi Directional Warp"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Warps input in multiple directions based on mask intensity.")); }
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Source")) FTG_Texture Source;
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Intensity Map")) FTG_Texture MaskTexture;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Intensity = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Rotation = 0.5f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float GradientAngle = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_NonUniformDirectionalWarp : public UTG_Expression {
	GENERATED_BODY()
public:
	TG_DECLARE_EXPRESSION(TG_Category::Filter);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Non-Uniform Directional Warp"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Warps input directionally masked by intensity.")); }
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Source")) FTG_Texture Source;
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Intensity Map")) FTG_Texture MaskTexture;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Intensity = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float GradientAngle = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_DirectionalDistance : public UTG_Expression {
	GENERATED_BODY()
public:
	TG_DECLARE_EXPRESSION(TG_Category::Filter);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Directional Distance"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Computes distance to nearest mask pixel directionally.")); }
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Source")) FTG_Texture Source;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Distance = 0.5f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float GradientAngle = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) int32 Samples = 32;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Threshold = 0.5f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_Cells1 : public UTG_Expression {
	GENERATED_BODY()
public:
	TG_DECLARE_EXPRESSION(TG_Category::Procedural);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Cells 1"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Scatters selected cell patterns with max blending.")); }
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "1", ClampMin = "1", UIMax = "256", ClampMax = "256")) int32 Scale = 16;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1")) float Disorder = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "8", ClampMax = "8")) float DisorderSpeed = 8.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1")) float DisorderAnisotropy = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float DisorderAnisotropyAngle = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) ETGMBCellsPattern Pattern = ETGMBCellsPattern::Disc;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) FVector2f PatternSize = FVector2f(0.8f, 0.8f);
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "8", ClampMax = "8")) float PatternScale = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1")) float LuminanceRandom = 0.35f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Angle = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1")) float AngleRandom = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) FVector2f TileOffset = FVector2f::ZeroVector;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) bool bNonSquareExpansion = true;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) int32 Seed = 0;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_Cells2 : public UTG_Expression {
	GENERATED_BODY()
public:
	TG_DECLARE_EXPRESSION(TG_Category::Procedural);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Cells 2"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Generates a binary cell mask with adjustable wall thickness.")); }
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "1", ClampMin = "1", UIMax = "256", ClampMax = "256")) int32 Scale = 16;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1")) float EdgeWidth = 0.05f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) bool bInvert = false;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1")) float Disorder = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "8", ClampMax = "8")) float DisorderSpeed = 8.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) FVector2f TileOffset = FVector2f::ZeroVector;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) bool bNonSquareExpansion = true;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) int32 Seed = 0;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_Cells3 : public UTG_Expression {
	GENERATED_BODY()
public:
	TG_DECLARE_EXPRESSION(TG_Category::Procedural);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Cells 3"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Generates soft uneven cell walls from intersecting discs.")); }
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "1", ClampMin = "1", UIMax = "256", ClampMax = "256")) int32 Scale = 16;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1")) float Hardness = 0.75f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) bool bInvert = false;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1")) float Disorder = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "8", ClampMax = "8")) float DisorderSpeed = 8.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1")) float DisorderAnisotropy = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float DisorderAnisotropyAngle = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) FVector2f PatternSize = FVector2f(0.9f, 0.9f);
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "8", ClampMax = "8")) float PatternScale = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Angle = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1")) float AngleRandom = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) FVector2f TileOffset = FVector2f::ZeroVector;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) bool bNonSquareExpansion = true;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) int32 Seed = 0;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_Cells4 : public UTG_Expression {
	GENERATED_BODY()
public:
	TG_DECLARE_EXPRESSION(TG_Category::Procedural);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Cells 4"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Assigns a flat grayscale value to each generated cell.")); }
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Input Grayscale")) FTG_Texture Input;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "1", ClampMin = "1", UIMax = "256", ClampMax = "256")) int32 Scale = 16;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1")) float Disorder = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "8", ClampMax = "8")) float DisorderSpeed = 8.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) ETGMBCells4ColorSource ColorSource = ETGMBCells4ColorSource::Random;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) int32 PseudorandomSeed = 0;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) FVector2f TileOffset = FVector2f::ZeroVector;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) bool bNonSquareExpansion = true;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) int32 Seed = 0;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_ShapeSplatter : public UTG_Expression {
	GENERATED_BODY()
public:
	TG_DECLARE_EXPRESSION(TG_Category::Procedural);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Shape Splatter"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Splatter shapes along a grid masked by a background map.")); }
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Pattern")) FTG_Texture Source;
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Background")) FTG_Texture MaskTexture;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float CountX = 8;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float CountY = 8;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Scale = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Threshold = 0.1f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_Clouds2 : public UTG_Expression {
	GENERATED_BODY()
public:
	TG_DECLARE_EXPRESSION(TG_Category::Procedural);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Clouds 2"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Generates cloud-like noise.")); }
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Scale = 4.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Contrast = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Bias = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float OffsetX = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float OffsetY = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) int32 Seed = 0;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_BnWSpots : public UTG_Expression {
	GENERATED_BODY()
public:
	TG_DECLARE_EXPRESSION(TG_Category::Procedural);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("BnW Spots"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Generates black and white spots.")); }
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Scale = 4.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Threshold = 0.5f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Smoothness = 0.1f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float OffsetX = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float OffsetY = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) int32 Seed = 0;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_GrungeDirt : public UTG_Expression {
	GENERATED_BODY()
public:
	TG_DECLARE_EXPRESSION(TG_Category::Procedural);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Grunge Dirt"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Generates grunge dirt noise.")); }
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Scale = 4.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Contrast = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Bias = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float OffsetX = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float OffsetY = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) int32 Seed = 0;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_Highpass : public UTG_Expression {
	GENERATED_BODY()
public:
	TG_DECLARE_EXPRESSION(TG_Category::Filter);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Highpass"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Applies a highpass filter.")); }
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Source")) FTG_Texture Source;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Radius = 4.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Contrast = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_LuminanceHighpass : public UTG_Expression {
	GENERATED_BODY()
public:
	TG_DECLARE_EXPRESSION(TG_Category::Filter);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Luminance Highpass"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Applies a luminance highpass filter.")); }
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Source")) FTG_Texture Source;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Radius = 4.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Contrast = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_BlurHQ : public UTG_Expression
{
	GENERATED_BODY()

public:
	TG_DECLARE_EXPRESSION(TG_Category::Filter);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Blur HQ"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Applies a high-quality Gaussian blur.")); }

	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Input"))
	FTG_Texture Input;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "16", ClampMax = "16"))
	float Intensity = 1.0f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1"))
	float Quality = 0.5f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = ""))
	FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_CurvatureSobel : public UTG_Expression {
	GENERATED_BODY()
public:
	TG_DECLARE_EXPRESSION(TG_Category::Filter);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Curvature Sobel"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Computes continuous Sobel edge strength, with an optional thresholded mask output.")); }
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Source")) FTG_Texture Source;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting"))
	ETGMBCurvatureSobelOutput OutputMode = ETGMBCurvatureSobelOutput::Strength;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "16", ClampMax = "64")) float Radius = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "4", ClampMax = "64")) float Intensity = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1")) float Threshold = 0.5f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "")) FTG_Texture Output;
};


UCLASS(MinimalAPI)
class UTG_Expression_TGMB_TileGenerator : public UTG_Expression
{
	GENERATED_BODY()

public:
	TG_DECLARE_EXPRESSION(TG_Category::Procedural);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Tile Generator"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Generates repeated procedural tile masks, IDs, and random colors.")); }

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting"))
	ETGMBTileGeneratorOutput OutputMode = ETGMBTileGeneratorOutput::Mask;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting"))
	ETGMBTilePattern Pattern = ETGMBTilePattern::Square;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "1", ClampMin = "1", UIMax = "128", ClampMax = "512"))
	int32 CountX = 8;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "1", ClampMin = "1", UIMax = "128", ClampMax = "512"))
	int32 CountY = 8;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0.01", ClampMin = "0.01", UIMax = "1", ClampMax = "2"))
	float Scale = 0.85f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "0.95", ClampMax = "0.95"))
	float Spacing = 0.05f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "-1", ClampMin = "-1", UIMax = "1", ClampMax = "1"))
	FVector2f Offset = FVector2f::ZeroVector;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "-3.14159", ClampMin = "-6.28318", UIMax = "3.14159", ClampMax = "6.28318"))
	float Rotation = 0.0f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1"))
	float PositionJitter = 0.0f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1"))
	float SizeJitter = 0.0f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "6.28318", ClampMax = "6.28318"))
	float RotationJitter = 0.0f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "2"))
	float LuminanceJitter = 0.0f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "100000", ClampMax = "2147483647"))
	int32 Seed = 0;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = ""))
	FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_TileSampler : public UTG_Expression
{
	GENERATED_BODY()

public:
	TG_DECLARE_EXPRESSION(TG_Category::Procedural);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Tile Sampler"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Samples an input texture into repeated cells with deterministic random variation.")); }

	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Source"))
	FTG_Texture Source;

	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Mask"))
	FTG_Texture Mask;

	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Scale Map"))
	FTG_Texture ScaleMap;

	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Rotation Map"))
	FTG_Texture RotationMap;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "1", ClampMin = "1", UIMax = "128", ClampMax = "512"))
	int32 CountX = 8;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "1", ClampMin = "1", UIMax = "128", ClampMax = "512"))
	int32 CountY = 8;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0.01", ClampMin = "0.01", UIMax = "1", ClampMax = "2"))
	float Scale = 0.85f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "0.95", ClampMax = "0.95"))
	float Spacing = 0.05f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "-3.14159", ClampMin = "-6.28318", UIMax = "3.14159", ClampMax = "6.28318"))
	float Rotation = 0.0f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1"))
	float PositionJitter = 0.0f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1"))
	float SizeJitter = 0.0f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "6.28318", ClampMax = "6.28318"))
	float RotationJitter = 0.0f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "100000", ClampMax = "2147483647"))
	int32 Seed = 0;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = ""))
	FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_GradientMap : public UTG_Expression
{
	GENERATED_BODY()

public:
	TG_DECLARE_EXPRESSION(TG_Category::Adjustment);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Gradient Map"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Maps grayscale input values through up to eight color stops.")); }

	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = ""))
	FTG_Texture Input;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "2", ClampMin = "2", UIMax = "8", ClampMax = "8"))
	int32 StopCount = 2;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting"))
	ETGMBGradientMapInterpolation Interpolation = ETGMBGradientMapInterpolation::Linear;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting"))
	ETGMBGradientMapAddressMode AddressMode = ETGMBGradientMapAddressMode::Clamp;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Position0 = 0.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) FLinearColor Color0 = FLinearColor::Black;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Position1 = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) FLinearColor Color1 = FLinearColor::White;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Position2 = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) FLinearColor Color2 = FLinearColor::White;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Position3 = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) FLinearColor Color3 = FLinearColor::White;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Position4 = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) FLinearColor Color4 = FLinearColor::White;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Position5 = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) FLinearColor Color5 = FLinearColor::White;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Position6 = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) FLinearColor Color6 = FLinearColor::White;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) float Position7 = 1.0f;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting")) FLinearColor Color7 = FLinearColor::White;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = ""))
	FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_NormalCombine : public UTG_Expression
{
	GENERATED_BODY()

public:
	TG_DECLARE_EXPRESSION(TG_Category::Adjustment);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Normal Combine"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Combines base and detail normal maps.")); }

	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Base"))
	FTG_Texture BaseNormal;

	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Detail"))
	FTG_Texture DetailNormal;

	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Mask"))
	FTG_Texture Mask;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting"))
	ETGMBNormalCombineMode Mode = ETGMBNormalCombineMode::RNM;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "2", ClampMax = "8"))
	float DetailStrength = 1.0f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting"))
	bool bInvertGreen = false;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = ""))
	FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_FloodFill : public UTG_Expression
{
	GENERATED_BODY()

public:
	TG_DECLARE_EXPRESSION(TG_Category::Filter);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Flood Fill"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Generates deterministic ID, random, position, and gradient data from separated mask regions.")); }

	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = ""))
	FTG_Texture Input;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting"))
	ETGMBFloodFillOutput OutputMode = ETGMBFloodFillOutput::ID;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting"))
	ETGMBFloodFillConnectivity Connectivity = ETGMBFloodFillConnectivity::EightWay;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1"))
	float Threshold = 0.5f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "-3.14159", ClampMin = "-6.28318", UIMax = "3.14159", ClampMax = "6.28318"))
	float GradientAngle = 0.0f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "100000", ClampMax = "2147483647"))
	int32 Seed = 0;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "Flood Fill Data"))
	FTG_Texture FloodFillData;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = ""))
	FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_SlopeBlur : public UTG_Expression
{
	GENERATED_BODY()

public:
	TG_DECLARE_EXPRESSION(TG_Category::Filter);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Slope Blur"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Blurs an input texture along the gradient of a slope texture.")); }

	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Input"))
	FTG_Texture Input;

	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Slope"))
	FTG_Texture Slope;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting"))
	ETGMBSlopeBlurMode Mode = ETGMBSlopeBlurMode::Blur;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "0.25", ClampMax = "2"))
	float Intensity = 0.025f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "1", ClampMin = "1", UIMax = "32", ClampMax = "64"))
	int32 Samples = 16;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting"))
	bool bInvertSlope = false;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = ""))
	FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_Bevel : public UTG_Expression
{
	GENERATED_BODY()

public:
	TG_DECLARE_EXPRESSION(TG_Category::Filter);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Bevel"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Creates a beveled height response around mask edges.")); }

	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = ""))
	FTG_Texture Input;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting"))
	ETGMBBevelMode Mode = ETGMBBevelMode::Both;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "0.1", ClampMax = "1"))
	float Distance = 0.02f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0.01", ClampMin = "0.01", UIMax = "8", ClampMax = "64"))
	float Smoothness = 1.0f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1"))
	float Threshold = 0.5f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = ""))
	FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_AmbientOcclusionHBAO : public UTG_Expression
{
	GENERATED_BODY()

public:
	TG_DECLARE_EXPRESSION(TG_Category::Adjustment);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Ambient Occlusion HBAO"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Approximates horizon-based ambient occlusion from a height map.")); }

	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Height"))
	FTG_Texture Height;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "0.25", ClampMax = "2"))
	float Radius = 0.05f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "1", ClampMin = "1", UIMax = "32", ClampMax = "64"))
	int32 Samples = 24;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "16", ClampMax = "64"))
	float HeightScale = 4.0f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "4"))
	float Bias = 0.02f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0.1", ClampMin = "0.1", UIMax = "4", ClampMax = "16"))
	float Contrast = 1.0f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting"))
	bool bInvert = false;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = ""))
	FTG_Texture Output;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_CurvatureSmooth : public UTG_Expression
{
	GENERATED_BODY()

public:
	TG_DECLARE_EXPRESSION(TG_Category::Adjustment);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Curvature Smooth"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Generates smoothed curvature, convex, and concave masks from a normal map.")); }

	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Normal"))
	FTG_Texture Normal;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "1", ClampMin = "1", UIMax = "16", ClampMax = "64"))
	float Radius = 1.0f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1"))
	float Smoothing = 0.5f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "8", ClampMax = "32"))
	float Intensity = 2.0f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "Curvature"))
	FTG_Texture Curvature;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "Convex"))
	FTG_Texture Convex;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "Concave"))
	FTG_Texture Concave;
};

UCLASS(MinimalAPI)
class UTG_Expression_TGMB_MultiMaterialBlend : public UTG_Expression
{
	GENERATED_BODY()

public:
	TG_DECLARE_EXPRESSION(TG_Category::Adjustment);
	virtual void Evaluate(FTG_EvaluationContext* InContext) override;
	virtual FTG_Name GetDefaultName() const override { return TEXT("Multi-Material Blend"); }
	virtual FText GetTooltipText() const override { return FText::FromString(TEXT("Blends two material channel bundles using a grayscale mask.")); }

	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "A BaseColor")) FTG_Texture ABaseColor;
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "A Normal")) FTG_Texture ANormal;
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "A ORM")) FTG_Texture AORM;
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "A Height")) FTG_Texture AHeight;
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "B BaseColor")) FTG_Texture BBaseColor;
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "B Normal")) FTG_Texture BNormal;
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "B ORM")) FTG_Texture BORM;
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "B Height")) FTG_Texture BHeight;
	UPROPERTY(meta = (TGType = "TG_Input", PinDisplayName = "Mask")) FTG_Texture Mask;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Setting", UIMin = "0", ClampMin = "0", UIMax = "1", ClampMax = "1"))
	float NormalStrength = 1.0f;

	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "BaseColor")) FTG_Texture BaseColor;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "Normal")) FTG_Texture Normal;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "ORM")) FTG_Texture ORM;
	UPROPERTY(EditAnywhere, Category = NoCategory, meta = (TGType = "TG_Output", PinDisplayName = "Height")) FTG_Texture Height;
};
