#include "PerformanceStatsLib.h"
#include "GenericPlatform/GenericPlatformMemory.h"

// BLOCCO IMPORTANTE PER WINDOWS
#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h" // Abilita i tipi Windows
#include <Pdh.h>
#include "Windows/HideWindowsPlatformTypes.h"  // Nasconde i tipi Windows per evitare conflitti con UE

// Non serve più #pragma comment(lib, "pdh.lib") perché lo abbiamo messo nel Build.cs
#endif

// Variabili statiche
#if PLATFORM_WINDOWS
static PDH_HQUERY CpuQuery;
static PDH_HCOUNTER CpuTotal;
#endif
static bool bPdhInitialized = false;

void UPerformanceStatsLib::InitPDHQueries()
{
#if PLATFORM_WINDOWS
    if (!bPdhInitialized)
    {
        // Apre la query
        if (PdhOpenQuery(NULL, 0, &CpuQuery) == ERROR_SUCCESS)
        {
            // Aggiunge il contatore per il tempo totale del processore
            // Nota: Usiamo PdhAddEnglishCounter per evitare problemi di lingua del sistema operativo
            PdhAddEnglishCounter(CpuQuery, TEXT("\\Processor(_Total)\\% Processor Time"), 0, &CpuTotal);
            PdhCollectQueryData(CpuQuery);
            bPdhInitialized = true;
        }
    }
#endif
}

float UPerformanceStatsLib::GetGPUFrameTime()
{
    // La variabile GAverageMS_GPU non è linkabile direttamente dai plugin esterni.
    // Usiamo invece GRenderThreadTime che è il tempo speso dal RenderThread.
    // Anche se tecnicamente è CPU-side, è l'indicatore principale del carico grafico.

    // GRenderThreadTime è una variabile globale accessibile
    return FPlatformTime::ToMilliseconds(GRenderThreadTime);
}

FSystemMetrics UPerformanceStatsLib::GetPerformanceMetrics(float DeltaTime)
{
    FSystemMetrics Metrics;

    // 1. FPS
    Metrics.FPS = (DeltaTime > 0.0f) ? (1.0f / DeltaTime) : 0.0f;

    // 2. Frame Time
    Metrics.FrameTime_ms = DeltaTime * 1000.0f;

    // 3. RAM (MB)
    FPlatformMemoryStats MemStats = FPlatformMemory::GetStats();
    Metrics.RAMUsage_MB = (float)MemStats.UsedPhysical / (1024.0f * 1024.0f);

    // 4. CPU Load %
    Metrics.CPULoad_Percent = 0.0f;

#if PLATFORM_WINDOWS
    if (bPdhInitialized)
    {
        PDH_FMT_COUNTERVALUE CounterVal;
        PdhCollectQueryData(CpuQuery);
        // Legge il valore formattato come double
        PDH_STATUS Status = PdhGetFormattedCounterValue(CpuTotal, PDH_FMT_DOUBLE, NULL, &CounterVal);

        if (Status == ERROR_SUCCESS)
        {
            Metrics.CPULoad_Percent = (float)CounterVal.doubleValue;
        }
    }
#endif

    return Metrics;
}