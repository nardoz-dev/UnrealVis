#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Http.h"
#include "PdbAsyncLoader.generated.h"

// La struttura dati (uguale a prima)
USTRUCT(BlueprintType)
struct FPdbInfo
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly) FString Title;
	UPROPERTY(BlueprintReadOnly) FString Classification;
	UPROPERTY(BlueprintReadOnly) FString Organism;
	UPROPERTY(BlueprintReadOnly) FString Date;
};

// Dichiariamo il "pin" di uscita del nodo
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPdbLoaderOutput, FPdbInfo, Data, FString, ErrorMessage);

UCLASS()
class UNREALVIS_API UPdbAsyncLoader : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	// I pin di uscita che vedrai nel Blueprint
	UPROPERTY(BlueprintAssignable)
	FPdbLoaderOutput OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FPdbLoaderOutput OnFail;

	// La funzione che crea il nodo
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "RCSB")
	static UPdbAsyncLoader* DownloadPdbInfo(FString PdbID);

	virtual void Activate() override;

private:
	FString TargetPdbID;
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};