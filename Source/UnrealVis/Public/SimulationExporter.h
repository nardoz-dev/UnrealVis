#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SimulationExporter.generated.h"

UCLASS()
class UNREALVIS_API USimulationExporter : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Esporta i dati.
	 * NOTA: Passiamo gli array separati perché la Struct è definita in Blueprint.
	 * NOTA: Passiamo le Opzioni come Stringhe perché l'Enum è definito in Blueprint.
	 */
	UFUNCTION(BlueprintCallable, Category = "Simulation | Export")
	static bool ExportSimulationToJson(
		FString FileName,
		FString SaveDirectory,
		// Dati della Struct MetricData "esplosi"
		const TArray<float>& FPS,
		const TArray<float>& FrameTime,
		const TArray<float>& CPULoad,
		const TArray<float>& RAMUsage,
		const TArray<float>& GPUFrameTime,
		// Metadati
		FString SimName,
		FString Description,
		bool bIsFreeType,
		FString TemplateUsed,
		// Enum convertito in stringhe dal Blueprint
		const TArray<FString>& OptionsUsedStrings
	);
};