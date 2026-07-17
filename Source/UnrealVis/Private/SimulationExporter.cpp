#include "SimulationExporter.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

bool USimulationExporter::ExportSimulationToJson(
	FString FileName,
	FString SaveDirectory,
	const TArray<float>& FPS,
	const TArray<float>& FrameTime,
	const TArray<float>& CPULoad,
	const TArray<float>& RAMUsage,
	const TArray<float>& GPUFrameTime,
	FString SimName,
	FString Description,
	bool bIsFreeType,
	FString TemplateUsed,
	const TArray<FString>& OptionsUsedStrings)
{
	// 1. Creiamo l'oggetto Root
	TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);

	// --- METADATI ---
	TSharedPtr<FJsonObject> MetadataObject = MakeShareable(new FJsonObject);
	MetadataObject->SetStringField("Name", SimName);
	MetadataObject->SetStringField("Description", Description);
	MetadataObject->SetStringField("Type", bIsFreeType ? TEXT("Free") : TEXT("Template"));
	MetadataObject->SetStringField("TemplateName", bIsFreeType ? TEXT("None") : TemplateUsed);

	RootObject->SetObjectField("Metadata", MetadataObject);

	// --- OPTIMIZATIONS (ENUM) ---
	// Qui prendiamo direttamente le stringhe passate dal Blueprint
	TArray<TSharedPtr<FJsonValue>> OptsArray;
	for (const FString& OptName : OptionsUsedStrings)
	{
		OptsArray.Add(MakeShareable(new FJsonValueString(OptName)));
	}
	RootObject->SetArrayField("Optimizations", OptsArray);

	// --- METRICHE ---
	TSharedPtr<FJsonObject> MetricsObject = MakeShareable(new FJsonObject);

	// Helper veloce per convertire float -> json
	auto ConvertFloatArray = [](const TArray<float>& InFloats) -> TArray<TSharedPtr<FJsonValue>>
		{
			TArray<TSharedPtr<FJsonValue>> OutArray;
			for (float Val : InFloats)
			{
				OutArray.Add(MakeShareable(new FJsonValueNumber(Val)));
			}
			return OutArray;
		};

	MetricsObject->SetArrayField("FPS", ConvertFloatArray(FPS));
	MetricsObject->SetArrayField("FrameTime", ConvertFloatArray(FrameTime));
	MetricsObject->SetArrayField("CPULoad", ConvertFloatArray(CPULoad));
	MetricsObject->SetArrayField("RAMUsage", ConvertFloatArray(RAMUsage));
	MetricsObject->SetArrayField("GPUFrameTime", ConvertFloatArray(GPUFrameTime));

	RootObject->SetObjectField("Metrics", MetricsObject);

	// --- SALVATAGGIO ---
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

	if (!FileName.EndsWith(".json")) FileName += ".json";
	FString FullPath = FPaths::Combine(SaveDirectory, FileName);

	return FFileHelper::SaveStringToFile(OutputString, *FullPath);
}