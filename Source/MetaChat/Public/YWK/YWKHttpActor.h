// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "YWKHttpActor.generated.h"

UCLASS()
class METACHAT_API AYWKHttpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AYWKHttpActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UYWKHttpUI> HttpUIFactory;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UMeetingButton> MeetingUIFactory;

	UPROPERTY(BlueprintReadWrite)
	class UMeetingButton* MeetingUI;

	UPROPERTY()
	class UYWKHttpUI* YWKHttpUI;

	// Send 버튼을 눌렀을때 요청할 함수
	void RsqGetTest(FString url);
	//응답받을 함수

	void OnRsqGetTest(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	void RsqPostTest(FString url, FString json);
	void OnResPostTest(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	// WAV 파일 전송
	bool LoadWavFileToBinary(const FString& FilePath, TArray<uint8>& OutBinaryData);
	void RsqPostwavfile(FString url, FString FilePath, FString MeetingId);

	int32 NewMeetingID;

	//window api viewer
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TSubclassOf<AActor> windowViewer;
};
