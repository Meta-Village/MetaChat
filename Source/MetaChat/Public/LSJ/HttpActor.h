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

	//Level에 따라 다른 UI 생성을 위해 Level명을 FString으로 초기화 
	//Level : 캐릭터커스텀 마이징 및 세션 생성/입장 - FString
	UPROPERTY(EditDefaultsOnly)
	FString CustomCharacterMap;
	//Level : 로그인/회원가입 - FString
	UPROPERTY(EditDefaultsOnly)
	FString RogoMap;

	//UI - Level : 캐릭터커스텀 마이징 및 세션 생성/입장 
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> CustomCharacterHttpUIFactory;
	UPROPERTY(BlueprintReadWrite)
	class USessionWidget* HttpUI;

	//UI - Level : 로그인/회원가입 - FString
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> LoginHttpUIFactory;
	class ULoginScreenWidget* LoginScreenHttpUI;

	// 테스트 Send 버튼을 눌렀을때 요청할 함수 : HTTP Get 방식
	void RsqGetTest(FString url);
	// 테스트 응답받을 함수 : HTTP Get 방식
	void OnRsqGetTest(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	// 테스트 Send 버튼을 눌렀을때 요청할 함수 : HTTP Post 방식
	void RsqPostTest(FString url, FString json);
	// 테스트 응답받을 함수 : HTTP Post 방식
	void OnResPostTest(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	// 세션 생성 송신 : HTTP Post 방식
	// Json 으로 세션명, 세션 비밀번호를 보낸다
	void RsqPostCreateWorld(FString url, FString json);
	// 세션 생성 통신 응답
	// ResultResponseCode와 Json으로 result값을 받는다.
	void OnResPostCreateWorld(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	// 세션 입장 송신 : HTTP Post 방식 - Json
	// 세션이 존재하는지, 세션의 비밀번호가 일치하는지 
	void RsqPostJoinWorld(FString url, FString json);
	// 세션 입장 통신 응답 :  - Json
	void OnRsqPostJoinWorld(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	// 아이디 생성 송신 : HTTP Post 방식 - Json
	void RsqPostCreateID(FString url, FString json);
	// 아이디 생성 통신 응답 :  - Json
	void OnRsqPostCreateID(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	// 로그인 송신 : HTTP Post 방식 - Json
	void RsqPostLogin(FString url, FString json);
	// 로그인 통신 응답 :  - Json
	void OnRsqPostLogin(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void RsqGetFindSession(FString url);
	void OnRsqGetFindSession(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

};
