#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PerformanceStatsLib.generated.h"

// Struct per passare tutti i dati in un colpo solo al Blueprint
USTRUCT(BlueprintType)
struct FSystemMetrics
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    float FPS;

    UPROPERTY(BlueprintReadOnly)
    float FrameTime_ms;

    UPROPERTY(BlueprintReadOnly)
    float CPULoad_Percent;

    UPROPERTY(BlueprintReadOnly)
    float RAMUsage_MB;
};

UCLASS()
class UNREALVIS_API UPerformanceStatsLib : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // Funzione da chiamare all'avvio del gioco
    UFUNCTION(BlueprintCallable, Category = "Performance")
    static void InitPDHQueries();

    // Funzione principale per CPU, RAM, FPS
    UFUNCTION(BlueprintCallable, Category = "Performance")
    static FSystemMetrics GetPerformanceMetrics(float DeltaTime);

    // --- NUOVA FUNZIONE GPU (Deve stare QUI dentro, prima della parentesi chiusa della classe) ---
    UFUNCTION(BlueprintPure, Category = "Performance")
    static float GetGPUFrameTime();


}; // <--- La classe finisce QUI. Niente UFUNCTION dopo questa riga!