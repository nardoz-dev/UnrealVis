#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadarChartWidget.generated.h"

USTRUCT(BlueprintType)
struct FRadarMetric
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxValue = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Color = FLinearColor::White;
};

UCLASS()
class UNREALVIS_API URadarChartWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// --- DATI ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar Data")
	TArray<FRadarMetric> Metrics;

	// NUOVO: Dati per la comparazione
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar Data")
	TArray<FRadarMetric> MetricsB;

	// --- CONFIGURAZIONE ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar Config")
	bool bShowSeriesB = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar Config")
	int32 GridSections = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar Config")
	float LineThickness = 2.0f;

	// --- STILE SERIE A ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar Style A")
	bool bDrawFill = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar Style A")
	FLinearColor FillColor = FLinearColor(0.0f, 1.0f, 0.0f, 0.3f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar Style A")
	FLinearColor OutlineColor = FLinearColor::Green;

	// --- STILE SERIE B ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar Style B")
	bool bDrawFillB = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar Style B")
	FLinearColor FillColorB = FLinearColor(0.0f, 0.5f, 1.0f, 0.3f); // Blu Trasparente

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar Style B")
	FLinearColor OutlineColorB = FLinearColor(0.0f, 1.0f, 1.0f, 1.0f); // Ciano

	// --- GENERALE ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar Style General")
	FLinearColor GridColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.2f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar Style General")
	FSlateFontInfo LabelFont;

	// --- INTERAZIONE ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar Interaction")
	bool bShowTooltipOnHover = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar Interaction")
	float HoverDistanceThreshold = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radar Interaction")
	FLinearColor TooltipBgColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.8f);

	// --- FUNZIONI ---
	UFUNCTION(BlueprintCallable, Category = "Radar Functions")
	void SetupStandardMetrics();
	
	UFUNCTION(BlueprintCallable, Category = "Radar Functions")
	void ClearMetrics();

	UFUNCTION(BlueprintCallable, Category = "Radar Functions")
	void ClearMetricsValues();

	// Aggiorna valori Serie A
	UFUNCTION(BlueprintCallable, Category = "Radar Functions")
	void UpdateMetricValue(int32 Index, float NewValue);

	// Aggiorna valori Serie B
	UFUNCTION(BlueprintCallable, Category = "Radar Functions")
	void UpdateMetricValueB(int32 Index, float NewValue);

protected:
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

private:
	FVector2D CachedMouseLocalPos = FVector2D(-1000, -1000);
	bool bIsMouseOver = false;

	// Funzione Helper per disegnare un poligono (Serie A o B)
	// Restituisce i punti calcolati (utile per il check del tooltip)
	TArray<FVector2D> DrawRadarSeries(
		const TArray<FRadarMetric>& InMetrics,
		const FGeometry& AllottedGeometry,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		FVector2D Center,
		float Radius,
		FLinearColor InOutlineColor,
		FLinearColor InFillColor,
		bool bFill) const;
};