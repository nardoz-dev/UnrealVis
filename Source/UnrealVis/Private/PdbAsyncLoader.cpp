#include "PdbAsyncLoader.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonSerializer.h"
#include "JsonObjectConverter.h"

UPdbAsyncLoader* UPdbAsyncLoader::DownloadPdbInfo(FString PdbID)
{
	UPdbAsyncLoader* Node = NewObject<UPdbAsyncLoader>();
	Node->TargetPdbID = PdbID;
	return Node;
}

void UPdbAsyncLoader::Activate()
{
	FString CleanID = TargetPdbID.TrimStartAndEnd();
	FString Url = FString::Printf(TEXT("https://data.rcsb.org/rest/v1/core/entry/%s"), *CleanID);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UPdbAsyncLoader::OnResponseReceived);
	Request->SetURL(Url);
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

void UPdbAsyncLoader::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FPdbInfo ResultData;
	FString ErrorMsg = "";

	if (!bWasSuccessful || !Response.IsValid())
	{
		ErrorMsg = "Connection Error";
		OnFail.Broadcast(ResultData, ErrorMsg);
		SetReadyToDestroy();
		return;
	}

	if (Response->GetResponseCode() != 200)
	{
		ErrorMsg = FString::Printf(TEXT("API Error: %d"), Response->GetResponseCode());
		OnFail.Broadcast(ResultData, ErrorMsg);
		SetReadyToDestroy();
		return;
	}

	// Parsing JSON
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		// Estrai Titolo
		TSharedPtr<FJsonObject> StructObj = JsonObject->GetObjectField("struct");
		if (StructObj.IsValid()) ResultData.Title = StructObj->GetStringField("title");

		// Estrai Keywords
		TSharedPtr<FJsonObject> KeywordsObj = JsonObject->GetObjectField("struct_keywords");
		if (KeywordsObj.IsValid()) ResultData.Classification = KeywordsObj->GetStringField("pdbx_keywords");

		// Estrai Organismo
		const TArray<TSharedPtr<FJsonValue>>* OrganismArray;
		if (JsonObject->TryGetArrayField("rcsb_entity_source_organism", OrganismArray) && OrganismArray->Num() > 0)
		{
			TSharedPtr<FJsonObject> OrgObj = (*OrganismArray)[0]->AsObject();
			if (OrgObj.IsValid()) ResultData.Organism = OrgObj->GetStringField("ncbi_scientific_name");
		}

		// Estrai Data
		TSharedPtr<FJsonObject> AccessionObj = JsonObject->GetObjectField("rcsb_accession_info");
		if (AccessionObj.IsValid()) ResultData.Date = AccessionObj->GetStringField("initial_release_date").Left(10);

		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("HTTP SUCCESS!"));

		OnSuccess.Broadcast(ResultData, "");
	}
	else
	{
		OnFail.Broadcast(ResultData, "JSON Parsing Failed");
	}

	SetReadyToDestroy(); // Pulisce la memoria
}