#include "AdvancedLineChart.h"
#include "Rendering/DrawElements.h"
#include "Blueprint/UserWidget.h"
#include "Framework/Application/SlateApplication.h" 

void UAdvancedLineChart::AddDataPoint(float NewValue, float CurrentTime, FString CustomLabel)
{
	FChartDataPoint NewPoint;
	NewPoint.Value = NewValue;
	NewPoint.Time = CurrentTime;
	NewPoint.Label = CustomLabel;
	DataPoints.Add(NewPoint);
}

void UAdvancedLineChart::AddDataPointB(float NewValue, float CurrentTime, FString CustomLabel)
{
	FChartDataPoint NewPoint;
	NewPoint.Value = NewValue;
	NewPoint.Time = CurrentTime;
	NewPoint.Label = CustomLabel;
	DataPointsB.Add(NewPoint);
}

void UAdvancedLineChart::ScrollToEnd()
{
	// Controlliamo chi ha più dati per gestire lo scroll correttamente
	int32 MaxPoints = FMath::Max(DataPoints.Num(), DataPointsB.Num());

	if (MaxPoints > PointsToDisplay)
	{
		ViewOffset = MaxPoints - PointsToDisplay;
	}
	else
	{
		ViewOffset = 0;
	}
}

int32 UAdvancedLineChart::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 BaseLayer = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	if (!ChartFont.HasValidFont()) return BaseLayer;
	FVector2D LocalSize = AllottedGeometry.GetLocalSize();
	if (LocalSize.X < 50 || LocalSize.Y < 50) return BaseLayer;

	// --- 1. DEFINIZIONE LAYOUT ---
	float GraphX = MarginLeft;
	float GraphY = 30.0f; // Spazio per titolo Y o margine alto
	float GraphW = LocalSize.X - MarginLeft - 20.0f; // Padding destro
	float GraphH = LocalSize.Y - MarginBottom - GraphY;

	float OriginX = GraphX;
	float OriginY = GraphY + GraphH;

	// --- 2. CALCOLO INDICI VISIBILI ---
	int32 TotalPointsA = DataPoints.Num();
	int32 TotalPointsB = DataPointsB.Num();

	// Indici Serie A
	int32 StartIdxA = FMath::Clamp(ViewOffset, 0, TotalPointsA);
	int32 EndIdxA = FMath::Clamp(StartIdxA + PointsToDisplay, 0, TotalPointsA);
	int32 CountA = EndIdxA - StartIdxA;
	bool bHasDataA = CountA >= 2;

	// Indici Serie B
	int32 StartIdxB = FMath::Clamp(ViewOffset, 0, TotalPointsB);
	int32 EndIdxB = FMath::Clamp(StartIdxB + PointsToDisplay, 0, TotalPointsB);
	int32 CountB = EndIdxB - StartIdxB;
	bool bHasDataB = (bShowSeriesB && CountB >= 2);

	if (!bHasDataA && !bHasDataB) return BaseLayer + 5;

	// --- 3. CALCOLO FINESTRA TEMPORALE (Fix Asse X) ---
	// Troviamo il tempo di inizio e fine visibile considerando ENTRAMBE le serie.
	// Questo serve per allineare le due linee e disegnare la griglia X correttamente.

	float VisibleStartTime = FLT_MAX;
	float VisibleEndTime = -FLT_MAX;
	bool bFoundTime = false;

	if (bHasDataA)
	{
		VisibleStartTime = FMath::Min(VisibleStartTime, DataPoints[StartIdxA].Time);
		// EndIdx è esclusivo, quindi prendiamo l'ultimo valido (EndIdx - 1)
		int32 LastIdx = FMath::Max(StartIdxA, EndIdxA - 1);
		VisibleEndTime = FMath::Max(VisibleEndTime, DataPoints[LastIdx].Time);
		bFoundTime = true;
	}

	if (bHasDataB)
	{
		VisibleStartTime = FMath::Min(VisibleStartTime, DataPointsB[StartIdxB].Time);
		int32 LastIdx = FMath::Max(StartIdxB, EndIdxB - 1);
		VisibleEndTime = FMath::Max(VisibleEndTime, DataPointsB[LastIdx].Time);
		bFoundTime = true;
	}

	if (!bFoundTime)
	{
		VisibleStartTime = 0.0f;
		VisibleEndTime = 1.0f;
	}

	// Se normalizziamo, useremo questo offset per far partire visivamente il grafico da 0
	float TimeNormalizationOffset = bNormalizeTime ? VisibleStartTime : 0.0f;

	// TimeRange: Durata esatta della finestra visualizzata
	float TimeRange = VisibleEndTime - VisibleStartTime;
	if (TimeRange <= 0.0001f) TimeRange = 1.0f;


	// --- 4. CALCOLO SCALE Y (Valori) ---
	float MinVal = 0.0f;
	float MaxVal = (FixedMaxY > 0) ? FixedMaxY : 100.0f;

	if (bAdaptiveScaleY)
	{
		float FoundMin = FLT_MAX;
		float FoundMax = -FLT_MAX;

		if (bHasDataA)
		{
			for (int32 i = StartIdxA; i < EndIdxA; ++i) {
				float Val = DataPoints[i].Value;
				if (Val < FoundMin) FoundMin = Val;
				if (Val > FoundMax) FoundMax = Val;
			}
		}
		if (bHasDataB)
		{
			for (int32 i = StartIdxB; i < EndIdxB; ++i) {
				float Val = DataPointsB[i].Value;
				if (Val < FoundMin) FoundMin = Val;
				if (Val > FoundMax) FoundMax = Val;
			}
		}

		if (FoundMin != FLT_MAX) {
			float Range = FoundMax - FoundMin;
			if (Range == 0) Range = 10.0f;
			// Arrotondamento a multipli di 5 per pulizia
			MinVal = FMath::FloorToFloat(FoundMin / 5.0f) * 5.0f;
			MaxVal = FMath::CeilToFloat(FoundMax / 5.0f) * 5.0f;
			if (MaxVal <= MinVal) MaxVal = MinVal + 10.0f;
		}
	}
	float ValRange = MaxVal - MinVal;


	// --- 5. DISEGNO GRIGLIA E ASSI ---

	// Linee Assi Esterne
	TArray<FVector2D> AxisLines;
	AxisLines.Add(FVector2D(OriginX, GraphY));
	AxisLines.Add(FVector2D(OriginX, OriginY));
	AxisLines.Add(FVector2D(OriginX + GraphW, OriginY));
	FSlateDrawElement::MakeLines(OutDrawElements, BaseLayer + 1, AllottedGeometry.ToPaintGeometry(), AxisLines, ESlateDrawEffect::None, AxisColor, false, 2.0f);

	// Titoli Assi
	FSlateDrawElement::MakeText(OutDrawElements, BaseLayer + 1, AllottedGeometry.ToPaintGeometry(FVector2D(100, 20), FSlateLayoutTransform(FVector2D(OriginX - 10, GraphY - 25))), LabelY, ChartFont, ESlateDrawEffect::None, TitleColor);
	FSlateDrawElement::MakeText(OutDrawElements, BaseLayer + 1, AllottedGeometry.ToPaintGeometry(FVector2D(100, 20), FSlateLayoutTransform(FVector2D(OriginX + (GraphW * 0.5f) - 20, OriginY + 25))), LabelX, ChartFont, ESlateDrawEffect::None, TitleColor);

	// --- GRIGLIA Y (Valori) ---
	int32 SafeYDivs = (YAxisDivisions > 0) ? YAxisDivisions : 1;
	for (int32 i = 0; i <= SafeYDivs; ++i)
	{
		float Alpha = (float)i / (float)SafeYDivs;
		float YPos = OriginY - (Alpha * GraphH);
		float Val = MinVal + (Alpha * ValRange);

		FString TextStr = FString::Printf(TEXT("%.0f"), Val);
		FVector2D TextPos(OriginX - 45.0f, YPos - 8.0f);
		FSlateDrawElement::MakeText(OutDrawElements, BaseLayer + 1, AllottedGeometry.ToPaintGeometry(FVector2D(40, 20), FSlateLayoutTransform(TextPos)), TextStr, ChartFont, ESlateDrawEffect::None, LabelColor);

		// Linea orizzontale
		FLinearColor GridColor = AxisColor; GridColor.A = 0.15f;
		TArray<FVector2D> GridLine = { FVector2D(OriginX, YPos), FVector2D(OriginX + GraphW, YPos) };
		FSlateDrawElement::MakeLines(OutDrawElements, BaseLayer, AllottedGeometry.ToPaintGeometry(), GridLine, ESlateDrawEffect::None, GridColor);

		// Tacchetta
		TArray<FVector2D> TickLine = { FVector2D(OriginX - 5, YPos), FVector2D(OriginX, YPos) };
		FSlateDrawElement::MakeLines(OutDrawElements, BaseLayer + 1, AllottedGeometry.ToPaintGeometry(), TickLine, ESlateDrawEffect::None, AxisColor);
	}

	// --- GRIGLIA X (Tempo Dinamico) ---
	int32 SafeXDivs = (XAxisDivisions > 0) ? XAxisDivisions : 1;
	for (int32 i = 0; i <= SafeXDivs; ++i)
	{
		float Alpha = (float)i / (float)SafeXDivs;
		float XPos = OriginX + (Alpha * GraphW);

		// Calcolo tempo reale in questo punto della griglia
		float ActualTime = VisibleStartTime + (Alpha * TimeRange);

		// Sottraiamo l'offset se normalizzazione è attiva
		float DisplayTime = ActualTime - TimeNormalizationOffset;

		FString TextStr = FString::Printf(TEXT("%.1f"), DisplayTime);
		FVector2D TextPos(XPos - 15.0f, OriginY + 5.0f);
		FSlateDrawElement::MakeText(OutDrawElements, BaseLayer + 1, AllottedGeometry.ToPaintGeometry(FVector2D(50, 20), FSlateLayoutTransform(TextPos)), TextStr, ChartFont, ESlateDrawEffect::None, LabelColor);

		// Tacchetta verticale
		TArray<FVector2D> TickLine = { FVector2D(XPos, OriginY), FVector2D(XPos, OriginY + 5.0f) };
		FSlateDrawElement::MakeLines(OutDrawElements, BaseLayer + 1, AllottedGeometry.ToPaintGeometry(), TickLine, ESlateDrawEffect::None, AxisColor);
	}

	// --- 6. DISEGNO SOGLIA (THRESHOLD) ---
	if (bShowThreshold)
	{
		float NormThresholdY = (ThresholdValue - MinVal) / ValRange;
		// Clamping per non disegnare fuori dal grafico
		if (NormThresholdY >= 0.0f && NormThresholdY <= 1.0f)
		{
			float ThreshY = OriginY - (NormThresholdY * GraphH);
			TArray<FVector2D> ThresholdPoints;
			ThresholdPoints.Add(FVector2D(OriginX, ThreshY));
			ThresholdPoints.Add(FVector2D(OriginX + GraphW, ThreshY));

			FSlateDrawElement::MakeLines(OutDrawElements, BaseLayer + 2,
				AllottedGeometry.ToPaintGeometry(),
				ThresholdPoints,
				ESlateDrawEffect::None,
				ThresholdColor,
				false,
				ThresholdThickness);
		}
	}

	// --- 7. DISEGNO SERIE DATI ---
	int32 CurrentLayer = BaseLayer + 3;

	// Passiamo 'VisibleStartTime' come GlobalStartTime a entrambe le funzioni
	// In questo modo entrambe le linee sono sincronizzate sulla stessa finestra temporale

	// Serie B (Sotto)
	if (bHasDataB)
	{
		DrawSeries(DataPointsB, StartIdxB, CountB,
			MinVal, ValRange, TimeRange,
			OriginX, OriginY, GraphW, GraphH,
			VisibleStartTime, // Global Start Time
			LineColorB, AreaFillColorB, PointColorB,
			bShowAreaFillB, bShowPoints,
			OutDrawElements, CurrentLayer, AllottedGeometry);
	}

	// Serie A (Sopra)
	if (bHasDataA)
	{
		DrawSeries(DataPoints, StartIdxA, CountA,
			MinVal, ValRange, TimeRange,
			OriginX, OriginY, GraphW, GraphH,
			VisibleStartTime, // Global Start Time
			LineColor, AreaFillColor, PointColor,
			bShowAreaFill, bShowPoints,
			OutDrawElements, CurrentLayer, AllottedGeometry);
	}

	return CurrentLayer;
}

void UAdvancedLineChart::ClearChart(bool bClearA, bool bClearB)
{
    // 1. Cancellazione Selettiva
    if (bClearA)
    {
        DataPoints.Empty();
    }

    if (bClearB)
    {
        DataPointsB.Empty();
    }

    // 2. Reset Intelligente dello Scorrimento
    // Resettiamo l'offset a 0 solo se il grafico è diventato completamente vuoto.
    // Se una delle due serie ha ancora dati, manteniamo la posizione attuale.
    if (DataPoints.Num() == 0 && DataPointsB.Num() == 0)
    {
        ViewOffset = 0;
    }
    // (Opzionale) Se cancelli la serie più lunga e rimani con quella corta, 
    // potresti voler clampare l'offset per non finire "oltre" i dati rimasti.
    else 
    {
        int32 MaxPoints = FMath::Max(DataPoints.Num(), DataPointsB.Num());
        if (ViewOffset > MaxPoints)
        {
            ViewOffset = FMath::Max(0, MaxPoints - PointsToDisplay);
        }
    }

    // 3. Aggiornamento Grafico
    InvalidateLayoutAndVolatility();
}

// -----------------------------------------------------------
// HELPER FUNCTION PER DISEGNO
// -----------------------------------------------------------
void UAdvancedLineChart::DrawSeries(const TArray<FChartDataPoint>& InData, int32 StartIdx, int32 Count,
	float MinVal, float ValRange, float TimeRange,
	float OriginX, float OriginY, float GraphW, float GraphH,
	float GlobalStartTime,
	FLinearColor InLineColor, FLinearColor InFillColor, FLinearColor InPointColor,
	bool bFill, bool bPoints,
	FSlateWindowElementList& OutDrawElements, int32& LayerId, const FGeometry& AllottedGeometry) const
{
	if (Count < 2) return;

	TArray<FVector2D> PointsTop;
	TArray<FVector2D> PointsBottom;

	// --- 1. Calcolo Coordinate ---
	for (int32 i = 0; i < Count; ++i)
	{
		// Check range
		if (!InData.IsValidIndex(StartIdx + i)) continue;

		const FChartDataPoint& Pt = InData[StartIdx + i];

		// Normalizzazione Valore Y
		float NormY = (Pt.Value - MinVal) / ValRange;

		// Normalizzazione Tempo X (Rispetto alla finestra visibile globale)
		float NormalizedTime = (Pt.Time - GlobalStartTime) / TimeRange;

		float X = OriginX + (NormalizedTime * GraphW);
		float Y = OriginY - (NormY * GraphH);

		// Clamp visivo opzionale (per non uscire dai bordi verticali)
		// Y = FMath::Clamp(Y, OriginY - GraphH, OriginY);

		PointsTop.Add(FVector2D(X, Y));
		PointsBottom.Add(FVector2D(X, OriginY));
	}

	// --- 2. Area Fill ---
	if (bFill && PointsTop.Num() > 1)
	{
		TArray<FSlateVertex> Vertices;
		TArray<SlateIndex> Indices;
		const FSlateRenderTransform& ChartTransform = AllottedGeometry.GetAccumulatedRenderTransform();

		for (int32 i = 0; i < PointsTop.Num(); ++i)
		{
			FColor TopColor = InFillColor.ToFColor(true);
			FColor BotColor = TopColor;
			BotColor.A = 0; // Sfumatura

			Vertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(ChartTransform, FVector2f(PointsTop[i]), FVector2f(0.0f, 0.0f), TopColor));
			Vertices.Add(FSlateVertex::Make<ESlateVertexRounding::Disabled>(ChartTransform, FVector2f(PointsBottom[i]), FVector2f(0.0f, 0.0f), BotColor));
		}

		for (int32 i = 0; i < PointsTop.Num() - 1; ++i)
		{
			int32 Top1 = i * 2;     int32 Bot1 = i * 2 + 1;
			int32 Top2 = (i + 1) * 2; int32 Bot2 = (i + 1) * 2 + 1;
			Indices.Add(Top1); Indices.Add(Bot1); Indices.Add(Bot2);
			Indices.Add(Top1); Indices.Add(Bot2); Indices.Add(Top2);
		}

		FSlateResourceHandle Handle = FSlateApplication::Get().GetRenderer()->GetResourceHandle(*FCoreStyle::Get().GetBrush("WhiteBrush"));
		FSlateDrawElement::MakeCustomVerts(OutDrawElements, LayerId, Handle, Vertices, Indices, nullptr, 0, 0);
	}

	// --- 3. Linea ---
	if (PointsTop.Num() > 1)
	{
		FSlateDrawElement::MakeLines(OutDrawElements, LayerId + 1, AllottedGeometry.ToPaintGeometry(), PointsTop, ESlateDrawEffect::None, InLineColor, true, LineThickness);
	}

	// --- 4. Punti e Label ---
	if (bPoints)
	{
		for (int32 i = 0; i < PointsTop.Num(); ++i)
		{
			FVector2D P = PointsTop[i];
			const FChartDataPoint& Pt = InData[StartIdx + i];

			bool bHasLabel = !Pt.Label.IsEmpty();
			float Size = bHasLabel ? PointSize * 1.5f : PointSize;
			FLinearColor Color = bHasLabel ? EventPointColor : InPointColor;

			FSlateDrawElement::MakeBox(OutDrawElements, LayerId + 2,
				AllottedGeometry.ToPaintGeometry(FVector2D(Size, Size), FSlateLayoutTransform(P - (Size * 0.5f))),
				FCoreStyle::Get().GetBrush("WhiteBrush"), ESlateDrawEffect::None, Color);

			if (bHasLabel)
			{
				FSlateDrawElement::MakeText(OutDrawElements, LayerId + 4,
					AllottedGeometry.ToPaintGeometry(FVector2D(100, 20), FSlateLayoutTransform(P + FVector2D(-10, -25))),
					Pt.Label, ChartFont, ESlateDrawEffect::None, EventPointColor);
			}

			if (bShowPointValues)
			{
				FString ValText = FString::SanitizeFloat(Pt.Value);
				if (ValText.Len() > 4) ValText = ValText.Left(4);
				FSlateDrawElement::MakeText(OutDrawElements, LayerId + 4,
					AllottedGeometry.ToPaintGeometry(FVector2D(50, 20), FSlateLayoutTransform(P + FVector2D(-10, -40))),
					ValText, ChartFont, ESlateDrawEffect::None, LabelColor);
			}
		}
	}

	// Incrementiamo LayerId per sovrapposizioni corrette
	LayerId += 5;
}