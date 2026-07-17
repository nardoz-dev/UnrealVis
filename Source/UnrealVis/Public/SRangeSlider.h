// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"
#include "Styling/SlateTypes.h" // Needed for FSliderStyle


// The internal delegate to talk to UMG
DECLARE_DELEGATE_TwoParams(FOnRangeValueChanged, float /*MinValue*/, float /*MaxValue*/);

class UNREALVIS_API SRangeSlider : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SRangeSlider)
        : _MinValue(0.0f)
        , _MaxValue(1.0f)
        , _WidgetStyle(&FCoreStyle::Get().GetWidgetStyle<FSliderStyle>("Slider"))
        // Valori di default
        , _MinThumbColor(FLinearColor::White)
        , _MaxThumbColor(FLinearColor::White)
        {}
        SLATE_ATTRIBUTE(float, MinValue)
        SLATE_ATTRIBUTE(float, MaxValue)
        SLATE_STYLE_ARGUMENT(FSliderStyle, WidgetStyle)
        // Nuovi argomenti per il colore
        SLATE_ATTRIBUTE(FLinearColor, MinThumbColor)
        SLATE_ATTRIBUTE(FLinearColor, MaxThumbColor)

        SLATE_ATTRIBUTE(FLinearColor, FillBarColor)

        SLATE_EVENT(FOnRangeValueChanged, OnValueChanged)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    // Overrides
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
    virtual FVector2D ComputeDesiredSize(float) const override;
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    void SetMinThumbColor(TAttribute<FLinearColor> InColor) { MinThumbColor = InColor; }
    void SetMaxThumbColor(TAttribute<FLinearColor> InColor) { MaxThumbColor = InColor; }
    void SetFillBarColor(TAttribute<FLinearColor> InColor) { FillBarColor = InColor; }

private:
    float MinValue;
    float MaxValue;
    const FSliderStyle* Style;

    bool bIsDraggingMin;
    bool bIsDraggingMax;

    FOnRangeValueChanged OnValueChanged;
    float PositionToValue(const FGeometry& MyGeometry, const FVector2D& AbsolutePos);

    TAttribute<FLinearColor> MinThumbColor;
    TAttribute<FLinearColor> MaxThumbColor;
    TAttribute<FLinearColor> FillBarColor;
};
