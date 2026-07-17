// Fill out your copyright notice in the Description page of Project Settings.


#include "SRangeSlider.h"
#include "Rendering/DrawElements.h"

void SRangeSlider::Construct(const FArguments& InArgs)
{
    MinValue = InArgs._MinValue.Get();
    MaxValue = InArgs._MaxValue.Get();
    Style = InArgs._WidgetStyle;
    OnValueChanged = InArgs._OnValueChanged;
    MinThumbColor = InArgs._MinThumbColor;
    MaxThumbColor = InArgs._MaxThumbColor;
    FillBarColor = InArgs._FillBarColor;
    bIsDraggingMin = false;
    bIsDraggingMax = false;
}

int32 SRangeSlider::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    FVector2D Size = AllottedGeometry.GetLocalSize();

    // Get images from the FSliderStyle
    const FSlateBrush* TrackBrush = &Style->NormalBarImage;
    const FSlateBrush* ThumbBrush = &Style->NormalThumbImage;

    float ThumbWidth = ThumbBrush->ImageSize.X;
    float UsableWidth = Size.X - ThumbWidth; // Keep thumbs inside bounds

    // --- Calculate fixed width of bar ---
    float BarThickness = Style->BarThickness;
    float TrackY = (Size.Y / 2.0f) - (BarThickness / 2.0f); 

    // 1. Draw Background Track
    FVector2D TrackPos(0.0f, TrackY);
    FVector2D TrackSize(Size.X, BarThickness);
    FSlateDrawElement::MakeBox(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(TrackSize, FSlateLayoutTransform(TrackPos)), TrackBrush, ESlateDrawEffect::None, InWidgetStyle.GetColorAndOpacityTint());

    // 2. Calculate Thumb Positions
    float MinX = (MinValue * UsableWidth) + (ThumbWidth / 2.0f);
    float MaxX = (MaxValue * UsableWidth) + (ThumbWidth / 2.0f);

    // 3. Draw Fill Bar
    float FillWidth = MaxX - MinX;
    FVector2D FillPos(MinX, TrackY);
    FVector2D FillSize(FillWidth, BarThickness);
    FSlateDrawElement::MakeBox(OutDrawElements, LayerId + 1, AllottedGeometry.ToPaintGeometry(FillSize, FSlateLayoutTransform(FillPos)), TrackBrush, ESlateDrawEffect::None, FillBarColor.Get() * InWidgetStyle.GetColorAndOpacityTint());

    // 3. Draw Min Thumb (Usa MinThumbColor)
    FVector2D MinThumbPos(MinX - (ThumbWidth / 2.0f), (Size.Y / 2.0f) - (ThumbBrush->ImageSize.Y / 2.0f));
    FSlateDrawElement::MakeBox(OutDrawElements, LayerId + 2, AllottedGeometry.ToPaintGeometry(ThumbBrush->ImageSize, FSlateLayoutTransform(MinThumbPos)), ThumbBrush, ESlateDrawEffect::None, MinThumbColor.Get() * InWidgetStyle.GetColorAndOpacityTint());

    // 4. Draw Max Thumb (Usa MaxThumbColor)
    FVector2D MaxThumbPos(MaxX - (ThumbWidth / 2.0f), (Size.Y / 2.0f) - (ThumbBrush->ImageSize.Y / 2.0f));
    FSlateDrawElement::MakeBox(OutDrawElements, LayerId + 2, AllottedGeometry.ToPaintGeometry(ThumbBrush->ImageSize, FSlateLayoutTransform(MaxThumbPos)), ThumbBrush, ESlateDrawEffect::None, MaxThumbColor.Get() * InWidgetStyle.GetColorAndOpacityTint());

    return LayerId + 2;
}

FVector2D SRangeSlider::ComputeDesiredSize(float) const
{
    return FVector2D(200.0f, Style->NormalThumbImage.ImageSize.Y);
}

FReply SRangeSlider::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        float NewVal = PositionToValue(MyGeometry, MouseEvent.GetScreenSpacePosition());

        // Which thumb is closer to the click?
        if (FMath::Abs(NewVal - MinValue) < FMath::Abs(NewVal - MaxValue))
        {
            bIsDraggingMin = true;
        }
        else
        {
            bIsDraggingMax = true;
        }
        return FReply::Handled().CaptureMouse(SharedThis(this));
    }
    return FReply::Unhandled();
}

FReply SRangeSlider::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && (bIsDraggingMin || bIsDraggingMax))
    {
        bIsDraggingMin = false;
        bIsDraggingMax = false;
        return FReply::Handled().ReleaseMouseCapture();
    }
    return FReply::Unhandled();
}

FReply SRangeSlider::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (!HasMouseCapture()) return FReply::Unhandled();

    float NewValue = PositionToValue(MyGeometry, MouseEvent.GetScreenSpacePosition());

    if (bIsDraggingMin)
    {
        MinValue = FMath::Clamp(NewValue, 0.0f, MaxValue);
        OnValueChanged.ExecuteIfBound(MinValue, MaxValue); // Send data!
    }
    else if (bIsDraggingMax)
    {
        MaxValue = FMath::Clamp(NewValue, MinValue, 1.0f);
        OnValueChanged.ExecuteIfBound(MinValue, MaxValue); // Send data!
    }

    return FReply::Handled();
}

float SRangeSlider::PositionToValue(const FGeometry& MyGeometry, const FVector2D& AbsolutePos)
{
    FVector2D LocalPos = MyGeometry.AbsoluteToLocal(AbsolutePos);
    float ThumbWidth = Style->NormalThumbImage.ImageSize.X;
    float UsableWidth = MyGeometry.GetLocalSize().X - ThumbWidth;

    // Adjust for thumb width so 0 and 1 hit the exact edges
    float AdjustedX = LocalPos.X - (ThumbWidth / 2.0f);
    return FMath::Clamp(AdjustedX / UsableWidth, 0.0f, 1.0f);
}