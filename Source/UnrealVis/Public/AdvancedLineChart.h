#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Fonts/SlateFontInfo.h"
#include "AdvancedLineChart.generated.h"

USTRUCT(BlueprintType)
struct FChartDataPoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Time;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Label;
};

UCLASS()
class UNREALVIS_API UAdvancedLineChart : public UUserWidget
{
	GENERATED_BODY()

public:
	// --- DATI SERIE A (Principale) ---
	UPROPERTY(BlueprintReadWrite, Category = "Chart Data")
	TArray<FChartDataPoint> DataPoints;

	// --- DATI SERIE B (Comparazione) ---
	UPROPERTY(BlueprintReadWrite, Category = "Chart Data")
	TArray<FChartDataPoint> DataPointsB;

	// --- CONFIGURAZIONE ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Scrolling")
	int32 PointsToDisplay = 60;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Scrolling")
	int32 ViewOffset = 0;

	// Se TRUE: Sottrae il tempo iniziale di ogni serie. Entrambe partiranno da 0 sull'asse X.
	// Fondamentale per comparare due giri/simulazioni diverse.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Comparison")
	bool bNormalizeTime = false;

	// Mostra la serie B?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Comparison")
	bool bShowSeriesB = false;

	// --- NUOVO: SOGLIA (THRESHOLD) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Threshold")
	bool bShowThreshold = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Threshold")
	float ThresholdValue = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Threshold")
	FLinearColor ThresholdColor = FLinearColor(1.0f, 0.5f, 0.0f, 1.0f); // Arancione di default

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Threshold")
	float ThresholdThickness = 3.0f;

	// --- STILE SERIE A ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Style A")
	bool bShowAreaFill = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Style A")
	FLinearColor AreaFillColor = FLinearColor(0.0f, 1.0f, 0.0f, 0.3f); // Verde trasparente

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Style A")
	FLinearColor LineColor = FLinearColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Style A")
	FLinearColor PointColor = FLinearColor::White;

	// --- STILE SERIE B ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Style B")
	bool bShowAreaFillB = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Style B")
	FLinearColor AreaFillColorB = FLinearColor(0.0f, 0.5f, 1.0f, 0.3f); // Blu trasparente

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Style B")
	FLinearColor LineColorB = FLinearColor(0.0f, 1.0f, 1.0f, 1.0f); // Ciano manuale

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Style B")
	FLinearColor PointColorB = FLinearColor(0.8f, 0.8f, 1.0f, 1.0f);

	// --- GENERALE ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Style General")
	float LineThickness = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Style General")
	bool bShowPoints = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Style General")
	bool bShowPointValues = false; // Mostra i numeri 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Style General")
	FLinearColor EventPointColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Style General")
	float PointSize = 6.0f;

	// --- ASSI E TESTI ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Axes")
	FLinearColor AxisColor = FLinearColor::Gray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Axes")
	FLinearColor LabelColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Axes")
	FLinearColor TitleColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Labels")
	FString LabelX = "Time (s)";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Labels")
	FString LabelY = "Value";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Labels")
	FSlateFontInfo ChartFont;

	// --- LAYOUT ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Layout")
	float MarginLeft = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Layout")
	float MarginBottom = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Layout")
	int32 YAxisDivisions = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Layout")
	int32 XAxisDivisions = 5;

	// --- SCALE ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Scales")
	bool bAdaptiveScaleY = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart Scales")
	float FixedMaxY = 100.0f;

	// --- FUNZIONI ---
	UFUNCTION(BlueprintCallable, Category = "Chart Data")
	void AddDataPoint(float NewValue, float CurrentTime, FString CustomLabel = "");

	// NUOVO: Aggiunta dati serie B
	UFUNCTION(BlueprintCallable, Category = "Chart Data")
	void AddDataPointB(float NewValue, float CurrentTime, FString CustomLabel = "");

	UFUNCTION(BlueprintCallable, Category = "Chart Scrolling")
	void ScrollToEnd();

	// Function to clear data chart
	UFUNCTION(BlueprintCallable, Category = "UnrealVis|Chart")
	void ClearChart(bool bClearA = true, bool bClearB = true);

	// Helper function per evitare codice duplicato nel disegno
	void DrawSeries(const TArray<FChartDataPoint>& InData, int32 StartIdx, int32 Count,
		float MinVal, float ValRange, float TimeRange,
		float OriginX, float OriginY, float GraphW, float GraphH,
		float TimeOffset, // Offset per la normalizzazione
		FLinearColor InLineColor, FLinearColor InFillColor, FLinearColor InPointColor,
		bool bFill, bool bPoints,
		FSlateWindowElementList& OutDrawElements, int32& LayerId, const FGeometry& AllottedGeometry) const;

protected:
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
};