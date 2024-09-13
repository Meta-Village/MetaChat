// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpActor.generated.h"

UCLASS()
class METACHAT_API AHttpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHttpActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	FString CustomCharacterMap;
	UPROPERTY(EditDefaultsOnly)
	FString RogoMap;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> CustomCharacterHttpUIFactory;
	UPROPERTY(BlueprintReadWrite)
	class USessionWidget* HttpUI;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> LoginHttpUIFactory;
	class ULoginScreenWidget* LoginScreenHttpUI;

	// Send 버튼을 눌렀을때 요청할 함수
	void RsqGetTest(FString url);
	//응답받을 함수
	void OnRsqGetTest(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	void RsqPostTest(FString url, FString json);
	void OnResPostTest(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	void RsqPostCreateWorld(FString url, FString json);
	void OnResPostCreateWorld(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void RsqPostJoinWorld(FString url, FString json);
	void OnRsqPostJoinWorld(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void RsqPostCreateID(FString url, FString json);
	void OnRsqPostCreateID(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void RsqPostLogin(FString url, FString json);
	void OnRsqPostLogin(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void RsqGetFindSession(FString url);
	void OnRsqGetFindSession(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

};
