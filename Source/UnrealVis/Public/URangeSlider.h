// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Styling/SlateTypes.h"
#include "URangeSlider.generated.h"


// THIS IS THE FUNCTION TO EXTRACT THE VALUES!
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRangeSliderValueChangedEvent, float, MinValue, float, MaxValue);

UCLASS()
class UNREALVIS_API URangeSlider : public UUserWidget
{
    GENERATED_BODY()

public:
    // Exposes the visual styling to the editor!
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
    FSliderStyle WidgetStyle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MinValue = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Values", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float MaxValue = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
    FLinearColor MinThumbColor = FLinearColor(0.0f, 0.5f, 1.0f, 1.0f); // Azzurro di default

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
    FLinearColor MaxThumbColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f); // Rosso di default

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
    FLinearColor FillBarColor = FLinearColor(0.0f, 0.5f, 1.0f, 1.0f); // Azzurro acceso di default

    // The Green Blueprint Event
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnRangeSliderValueChangedEvent OnValueChanged;

    URangeSlider(const FObjectInitializer& ObjectInitializer);

    virtual void ReleaseSlateResources(bool bReleaseChildren) override;

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

    virtual void SynchronizeProperties() override;

private:
    TSharedPtr<class SRangeSlider> MyRangeSlider;
    void HandleOnValueChanged(float InMin, float InMax);
};