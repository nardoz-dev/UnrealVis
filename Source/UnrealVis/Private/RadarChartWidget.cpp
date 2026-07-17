#include "RadarChartWidget.h"
#include "Rendering/DrawElements.h"
#include "Slate/SlateBrushAsset.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void URadarChartWidget::SetupStandardMetrics()
{
	// Setup standard per Serie A
	Metrics.Empty();
	Metrics.Add({ "FPS", 0.0f, 120.0f, FLinearColor::Green });
	Metrics.Add({ "Frame Time", 0.0f, 33.0f, FLinearColor::Red });
	Metrics.Add({ "CPU Load", 0.0f, 100.0f, FLinearColor::Yellow });
	Metrics.Add({ "RAM", 0.0f, 16000.0f, FLinearColor::Blue });
	Metrics.Add({ "GPU Time", 0.0f, 33.0f, FLinearColor(1.0f, 0.0f, 1.0f, 1.0f) });

	// Copiamo la struttura in Serie B per comodità (valori a 0)
	MetricsB = Metrics;
	for (auto& M : MetricsB) M.Value = 0.0f;
}

void URadarChartWidget::UpdateMetricValue(int32 Index, float NewValue)
{
	if (Metrics.IsValidIndex(Index)) Metrics[Index].Value = NewValue;
}

void URadarChartWidget::UpdateMetricValueB(int32 Index, float NewValue)
{
	if (MetricsB.IsValidIndex(Index)) MetricsB[Index].Value = NewValue;
}

FReply URadarChartWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	CachedMouseLocalPos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	bIsMouseOver = true;
	Invalidate(EInvalidateWidgetReason::Paint); // Forza ridisegno
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void URadarChartWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	bIsMouseOver = false;
	CachedMouseLocalPos = FVector2D(-1000, -1000);
	Invalidate(EInvalidateWidgetReason::Paint);
	Super::NativeOnMouseLeave(InMouseEvent);
}

// 1. Svuota tutto (Tabula Rasa)
void URadarChartWidget::ClearMetrics()
{
	Metrics.Empty();
	MetricsB.Empty();

	// Opzionale: Forza il ridisegno immediato per vedere il grafico sparire
	Invalidate(EInvalidateWidgetReason::Paint);
}

// 2. Azzera solo i valori (Reset della sessione)
void URadarChartWidget::ClearMetricsValues()
{
	// Azzera Serie A
	for (auto& M : Metrics)
	{
		M.Value = 0.0f;
	}

	// Azzera Serie B
	for (auto& M : MetricsB)
	{
		M.Value = 0.0f;
	}

	// Forza il ridisegno per vedere il grafico collassare al centro
	Invalidate(EInvalidateWidgetReason::Paint);
}

int32 URadarChartWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 BaseLayer = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	if (Metrics.Num() < 3) return BaseLayer;

	FVector2D LocalSize = AllottedGeometry.GetLocalSize();
	FVector2D Center = LocalSize * 0.5f;
	float Radius = FMath::Min(LocalSize.X, LocalSize.Y) * 0.45f;

	int32 NumPoints = Metrics.Num();
	float AngleStep = 2.0f * PI / NumPoints;
	float StartAngle = -PI / 2.0f;

	// ---------------------------------------------------------
	// 1. GRIGLIA E ASSI (Comune)
	// ---------------------------------------------------------
	// Rombi concentrici
	for (int32 i = 1; i <= GridSections; ++i)
	{
		float CurrentRadius = Radius * ((float)i / (float)GridSections);
		TArray<FVector2D> GridPoints;
		for (int32 j = 0; j <= NumPoints; ++j)
		{
			float Angle = StartAngle + ((j % NumPoints) * AngleStep);
			GridPoints.Add(Center + FVector2D(FMath::Cos(Angle), FMath::Sin(Angle)) * CurrentRadius);
		}
		FSlateDrawElement::MakeLines(OutDrawElements, BaseLayer + 1, AllottedGeometry.ToPaintGeometry(), GridPoints, ESlateDrawEffect::None, GridColor, true, 1.0f);
	}

	// Assi e Testi
	for (int32 i = 0; i < NumPoints; ++i)
	{
		float Angle = StartAngle + (i * AngleStep);
		FVector2D Dir(FMath::Cos(Angle), FMath::Sin(Angle));
		FVector2D AxisEnd = Center + Dir * Radius;

		TArray<FVector2D> AxisLine = { Center, AxisEnd };
		FSlateDrawElement::MakeLines(OutDrawElements, BaseLayer + 1, AllottedGeometry.ToPaintGeometry(), AxisLine, ESlateDrawEffect::None, GridColor, false, 1.0f);

		if (LabelFont.HasValidFont())
		{
			FVector2D TextPos = AxisEnd + (Dir * 20.0f) - FVector2D(15, 10);
			FSlateDrawElement::MakeText(OutDrawElements, BaseLayer + 1, AllottedGeometry.ToPaintGeometry(FVector2D(100, 30), FSlateLayoutTransform(TextPos)), Metrics[i].Name, LabelFont, ESlateDrawEffect::None, Metrics[i].Color);
		}
	}

	// ---------------------------------------------------------
	// 2. DISEGNO SERIE DATI
	// ---------------------------------------------------------
	TArray<FVector2D> PointsA;
	TArray<FVector2D> PointsB;

	// Serie B (Sotto)
	if (bShowSeriesB && MetricsB.Num() == Metrics.Num())
	{
		PointsB = DrawRadarSeries(MetricsB, AllottedGeometry, OutDrawElements, BaseLayer + 2, Center, Radius, OutlineColorB, FillColorB, bDrawFillB);
	}

	// Serie A (Sopra)
	PointsA = DrawRadarSeries(Metrics, AllottedGeometry, OutDrawElements, BaseLayer + 5, Center, Radius, OutlineColor, FillColor, bDrawFill);


	// ---------------------------------------------------------
	// 3. TOOLTIP AVANZATO (Cerca su entrambe le serie)
	// ---------------------------------------------------------
	if (bShowTooltipOnHover && bIsMouseOver)
	{
		float MinDist = HoverDistanceThreshold;
		int32 FoundIndex = -1;
		bool bFoundInB = false;
		FVector2D FoundPos;

		// Cerca in A
		for (int32 i = 0; i < PointsA.Num(); ++i)
		{
			float Dist = FVector2D::Distance(PointsA[i], CachedMouseLocalPos);
			if (Dist < MinDist)
			{
				MinDist = Dist;
				FoundIndex = i;
				FoundPos = PointsA[i];
				bFoundInB = false;
			}
		}

		// Cerca in B (se attiva)
		if (bShowSeriesB && PointsB.Num() > 0)
		{
			for (int32 i = 0; i < PointsB.Num(); ++i)
			{
				float Dist = FVector2D::Distance(PointsB[i], CachedMouseLocalPos);
				if (Dist < MinDist)
				{
					MinDist = Dist;
					FoundIndex = i;
					FoundPos = PointsB[i];
					bFoundInB = true; // Segnaliamo che è della serie B
				}
			}
		}

		// Se abbiamo trovato un punto vicino
		if (FoundIndex != -1)
		{
			// Recuperiamo il dato corretto
			const FRadarMetric& RefMetric = bFoundInB ? MetricsB[FoundIndex] : Metrics[FoundIndex];
			FLinearColor TextCol = bFoundInB ? OutlineColorB : OutlineColor;

			FString ValText = FString::SanitizeFloat(RefMetric.Value);
			if (bFoundInB) ValText = "B: " + ValText; // Prefisso opzionale per capire chi sto guardando

			// Disegna Tooltip
			FVector2D TooltipPos = FoundPos + FVector2D(15, -15);
			FSlateDrawElement::MakeBox(OutDrawElements, BaseLayer + 20, AllottedGeometry.ToPaintGeometry(FVector2D(80, 25), FSlateLayoutTransform(TooltipPos)), FCoreStyle::Get().GetBrush("WhiteBrush"), ESlateDrawEffect::None, TooltipBgColor);
			FSlateDrawElement::MakeText(OutDrawElements, BaseLayer + 21, AllottedGeometry.ToPaintGeometry(FVector2D(80, 25), FSlateLayoutTransform(TooltipPos + FVector2D(5, 5))), ValText, LabelFont, ESlateDrawEffect::None, FLinearColor::White);

			// Pallino di evidenziazione
			FSlateDrawElement::MakeBox(OutDrawElements, BaseLayer + 21, AllottedGeometry.ToPaintGeometry(FVector2D(8, 8), FSlateLayoutTransform(FoundPos - FVector2D(4, 4))), FCoreStyle::Get().GetBrush("WhiteBrush"), ESlateDrawEffect::None, TextCol);
		}
	}

	return BaseLayer + 25;
}

// ---------------------------------------------------------
// Helper per il disegno (Area + Outline)
// ---------------------------------------------------------
TArray<FVector2D> URadarChartWidget::DrawRadarSeries(
	const TArray<FRadarMetric>& InMetrics,
	const FGeometry& AllottedGeometry,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	FVector2D Center,
	float Radius,
	FLinearColor InOutlineColor,
	FLinearColor InFillColor,
	bool bFill) const
{
	TArray<FVector2D> CalculatedPoints;
	int32 NumPoints = InMetrics.Num();
	float AngleStep = 2.0f * PI / NumPoints;
	float StartAngle = -PI / 2.0f;

	// 1. Calcolo Punti
	for (int32 i = 0; i < NumPoints; ++i)
	{
		float Angle = StartAngle + (i * AngleStep);
		float Normalized = FMath::Clamp(InMetrics[i].Value / InMetrics[i].MaxValue, 0.0f, 1.0f);
		FVector2D Point = Center + FVector2D(FMath::Cos(Angle), FMath::Sin(Angle)) * (Radius * Normalized);
		CalculatedPoints.Add(Point);
	}

	// 2. Area Fill
	if (bFill && CalculatedPoints.Num() >= 3)
	{
		TArray<FSlateVertex> Vertices;
		TArray<SlateIndex> Indices;
		const FSlateRenderTransform& ChartTransform = AllottedGeometry.GetAccumulatedRenderTransform();
		FColor FinalFill = InFillColor.ToFColor(true);

		Vertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(ChartTransform, FVector2f(Center), FVector2f(0, 0), FinalFill));
		for (const FVector2D& P : CalculatedPoints)
		{
			Vertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(ChartTransform, FVector2f(P), FVector2f(0, 0), FinalFill));
		}

		for (int32 i = 0; i < NumPoints; ++i)
		{
			Indices.Add(0);
			Indices.Add(i + 1);
			int32 NextIdx = (i == NumPoints - 1) ? 1 : (i + 2);
			Indices.Add(NextIdx);
		}
		FSlateResourceHandle Handle = FSlateApplication::Get().GetRenderer()->GetResourceHandle(*FCoreStyle::Get().GetBrush("WhiteBrush"));
		FSlateDrawElement::MakeCustomVerts(OutDrawElements, LayerId, Handle, Vertices, Indices, nullptr, 0, 0);
	}

	// 3. Outline (chiudi il cerchio)
	if (CalculatedPoints.Num() >= 2)
	{
		TArray<FVector2D> OutlinePts = CalculatedPoints;
		OutlinePts.Add(CalculatedPoints[0]);
		FSlateDrawElement::MakeLines(OutDrawElements, LayerId + 1, AllottedGeometry.ToPaintGeometry(), OutlinePts, ESlateDrawEffect::None, InOutlineColor, true, LineThickness);
	}

	return CalculatedPoints;
}