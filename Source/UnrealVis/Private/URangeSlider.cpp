#include "URangeSlider.h"
#include "SRangeSlider.h"

URangeSlider::URangeSlider(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Load default styling from the engine so it isn't invisible when you first drag it in!
    WidgetStyle = FCoreStyle::Get().GetWidgetStyle<FSliderStyle>("Slider");
}

TSharedRef<SWidget> URangeSlider::RebuildWidget()
{
    MyRangeSlider = SNew(SRangeSlider)
        .WidgetStyle(&WidgetStyle)
        .MinValue(MinValue)
        .MaxValue(MaxValue)
        .MinThumbColor(MinThumbColor) 
        .MaxThumbColor(MaxThumbColor)
        .FillBarColor(FillBarColor)
        .OnValueChanged(BIND_UOBJECT_DELEGATE(FOnRangeValueChanged, HandleOnValueChanged));

    return MyRangeSlider.ToSharedRef();
}

void URangeSlider::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);
    MyRangeSlider.Reset();
}

void URangeSlider::HandleOnValueChanged(float InMin, float InMax)
{
    MinValue = InMin;
    MaxValue = InMax;
    OnValueChanged.Broadcast(InMin, InMax); // Pushes the values to Blueprint
}

void URangeSlider::SynchronizeProperties()
{
    Super::SynchronizeProperties();

    // Questo fa sì che se cambi il colore nel pannello Details, si aggiorna subito nel designer!
    if (MyRangeSlider.IsValid())
    {
        MyRangeSlider->SetMinThumbColor(MinThumbColor);
        MyRangeSlider->SetMaxThumbColor(MaxThumbColor);
        MyRangeSlider->SetFillBarColor(FillBarColor);
    }
}