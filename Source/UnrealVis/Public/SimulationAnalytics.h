#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SimulationAnalytics.generated.h"

// Enum per dire chi ha vinto in modo leggibile
UENUM(BlueprintType)
enum class EComparisonWinner : uint8
{
    SimulationA     UMETA(DisplayName = "Simulation A Wins"),
    SimulationB     UMETA(DisplayName = "Simulation B Wins"),
    Tie             UMETA(DisplayName = "Tie (Equal)")
};

// Struct che contiene tutti i dati del confronto
USTRUCT(BlueprintType)
struct FComparisonResult
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EComparisonWinner Winner = EComparisonWinner::Tie;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MeanA = 0.0f; // Media della prima simulazione

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MeanB = 0.0f; // Media della seconda simulazione

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DeltaPercent = 0.0f; // Differenza in percentuale (es. +15%)

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DeltaValue = 0.0f; // Differenza assoluta (es. +5.2 FPS)

    UPROPERTY(BlueprintReadOnly)
    float DominanceRatioA;
};

UCLASS()
class UNREALVIS_API USimulationAnalytics : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Compara due array di metriche calcolando la media.
     * Gestisce array di dimensioni diverse.
     * * @param DataA - Array della prima simulazione
     * @param DataB - Array della seconda simulazione
     * @param bHigherIsBetter - Se TRUE (FPS), vince chi ha la media più alta. Se FALSE (CPU/RAM/Time), vince chi ha la media più bassa.
     */
    UFUNCTION(BlueprintCallable, Category = "Simulation | Analytics")
    static FComparisonResult CompareMetrics(const TArray<float>& DataA, const TArray<float>& DataB, bool bHigherIsBetter);

private:
    // Helper function interna per calcolare la media
    static float CalculateMean(const TArray<float>& Data);
};