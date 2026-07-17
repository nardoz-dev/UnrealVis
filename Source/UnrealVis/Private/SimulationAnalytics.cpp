#include "SimulationAnalytics.h"

FComparisonResult USimulationAnalytics::CompareMetrics(const TArray<float>& DataA, const TArray<float>& DataB, bool bHigherIsBetter)
{
    FComparisonResult Result;

    // 1. Calcolo delle Medie (Indipendente dalla dimensione degli array)
    Result.MeanA = CalculateMean(DataA);
    Result.MeanB = CalculateMean(DataB);

    // 2. Calcolo Differenza Assoluta
    Result.DeltaValue = FMath::Abs(Result.MeanA - Result.MeanB);
    float BaseValue = FMath::Min(Result.MeanA, Result.MeanB);
    Result.DeltaPercent = (BaseValue > KINDA_SMALL_NUMBER) ? (Result.DeltaValue / BaseValue) * 100.0f : 0.0f;


    // 4. Determinare il Vincitore
    // Caso Parità
    if (FMath::IsNearlyEqual(Result.MeanA, Result.MeanB, 0.001f))
    {
        Result.Winner = EComparisonWinner::Tie;
        Result.DominanceRatioA = 0.5f; // Perfettamente bilanciato
        return Result;
    }

    if (bHigherIsBetter)
    {
        Result.Winner = (Result.MeanA > Result.MeanB) ? EComparisonWinner::SimulationA : EComparisonWinner::SimulationB;

        // Calcolo Dominio (FPS): Chi ne ha di più, prende più spazio
        float Total = Result.MeanA + Result.MeanB;
        if (Total > KINDA_SMALL_NUMBER)
            Result.DominanceRatioA = Result.MeanA / Total;
        else
            Result.DominanceRatioA = 0.5f;
    }
    else
    {
        Result.Winner = (Result.MeanA < Result.MeanB) ? EComparisonWinner::SimulationA : EComparisonWinner::SimulationB;


        float ScoreA = Result.MeanB;
        float ScoreB = Result.MeanA;
        float Total = ScoreA + ScoreB;

        if (Total > KINDA_SMALL_NUMBER)
            Result.DominanceRatioA = ScoreA / Total;
        else
            Result.DominanceRatioA = 0.5f;
    }

    return Result;
}

float USimulationAnalytics::CalculateMean(const TArray<float>& Data)
{
    if (Data.Num() == 0) return 0.0f;

    double Sum = 0.0; // Usiamo double per precisione durante la somma
    for (float Val : Data)
    {
        Sum += Val;
    }

    return (float)(Sum / Data.Num());
}