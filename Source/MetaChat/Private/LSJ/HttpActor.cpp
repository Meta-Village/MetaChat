﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "LSJ/HttpActor.h"
#include "LSJ/SessionWidget.h"
#include <HttpModule.h>
#include "LSJ/JsonParseLib.h"

#include "Engine/Texture2D.h"
#include "Engine/Texture2DDynamic.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#include <vector> 

#include "Kismet/GameplayStatics.h"
#include "LSJ/LoginScreenWidget.h"
// Sets default values
AHttpActor::AHttpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AHttpActor::BeginPlay()
{
	Super::BeginPlay();
	
	FString CurrentWorldName =UGameplayStatics::GetCurrentLevelName(GetWorld(),true);
	if (CurrentWorldName == CustomCharacterMap)
	{
		HttpUI = Cast<USessionWidget>(CreateWidget(GetWorld(), CustomCharacterHttpUIFactory));
		if (HttpUI)
		{
			HttpUI->AddToViewport(2);
			HttpUI->SetHttpActor(this);
		}
	}
	else if (CurrentWorldName == RogoMap)
	{
		LoginScreenHttpUI = Cast<ULoginScreenWidget>(CreateWidget(GetWorld(), LoginHttpUIFactory));
		if (LoginScreenHttpUI)
		{
			LoginScreenHttpUI->AddToViewport();
			LoginScreenHttpUI->SetHttpActor(this);
		}
	}else if (CurrentWorldName == "PrototypeMain")
	{

	}

}

// Called every frame
void AHttpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if(HttpUI)
	//	HttpUI->UpdateWidgetTexture();
}

void AHttpActor::RsqGetTest(FString url)
{
	//Http 모듈 생성
	FHttpModule& httpModule = FHttpModule::Get();
	//IHttpRequest = TSharedRef<IHttpRequest> //스마트 포인터 //댕글링포인터가 되는것을 방지
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();

	//요청할 정보를 설정
	req->SetURL(url);
	req->SetVerb(TEXT("GET"));
	req->SetHeader(TEXT("Content-Type"), TEXT("Application/json")); //Http Content type 
	//응답받을 함수를 연결
	req->OnProcessRequestComplete().BindUObject(this, &AHttpActor::OnRsqGetTest);
	//서버에 요청
	req->ProcessRequest();
}


void AHttpActor::OnRsqGetTest(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		//통신성공
		FString result = Response->GetContentAsString(); //어떤 타입으로 받을 것인지
		int32 ResultResponseCode = Response->GetResponseCode();

	}
	else
	{
		//통신성공
		UE_LOG(LogTemp, Error, TEXT("bConnectedSuccessfully Fail.."));
	}
}

void AHttpActor::RsqPostTest(FString url, FString json)
{
	//Http 모듈 생성
	FHttpModule& httpModule = FHttpModule::Get();
	//IHttpRequest = TSharedRef<IHttpRequest> //스마트 포인터 //댕글링포인터가 되는것을 방지
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();

	//요청할 정보를 설정
	req->SetURL(url);
	req->SetVerb(TEXT("POST"));
	req->SetHeader(TEXT("Content-Type"), TEXT("Application/json")); //Http Content type 
	req->SetContentAsString(json);
	//응답받을 함수를 연결
	req->OnProcessRequestComplete().BindUObject(this, &AHttpActor::OnResPostTest);
	//서버에 요청
	req->ProcessRequest();
}

void AHttpActor::OnResPostTest(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		//통신성공
		FString result = Response->GetContentAsString(); //어떤 타입으로 받을 것인지

	}
	else
	{
		//통신성공
		UE_LOG(LogTemp, Error, TEXT("OnResPostTest Fail.."));
	}
}

void AHttpActor::RsqPostCreateWorld(FString url, FString json)
{
	//Http 모듈 생성
	FHttpModule& httpModule = FHttpModule::Get();
	//IHttpRequest = TSharedRef<IHttpRequest> //스마트 포인터 //댕글링포인터가 되는것을 방지
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();

	//요청할 정보를 설정
	req->SetURL(url);
	req->SetVerb(TEXT("POST"));
	req->SetHeader(TEXT("Content-Type"), TEXT("Application/json")); //Http Content type 
	req->SetContentAsString(json);
	//응답받을 함수를 연결
	req->OnProcessRequestComplete().BindUObject(this, &AHttpActor::OnResPostCreateWorld);
	//서버에 요청
	req->ProcessRequest();
}

void AHttpActor::OnResPostCreateWorld(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		//통신성공
		FString result = Response->GetContentAsString(); //어떤 타입으로 받을 것인지
		int32 ResultResponseCode = Response->GetResponseCode();
		//필요한 정보를 뽑아서 화면에 출력하고 싶다.
		HttpUI->RecvCreatingWorldInfo(result,ResultResponseCode);
	}
	else
	{
		//통신성공
		UE_LOG(LogTemp, Error, TEXT("OnResPostTest Fail.."));
	}
}
void AHttpActor::RsqPostJoinWorld(FString url, FString json)
{
	//Http 모듈 생성
	FHttpModule& httpModule = FHttpModule::Get();
	//IHttpRequest = TSharedRef<IHttpRequest> //스마트 포인터 //댕글링포인터가 되는것을 방지
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();

	//요청할 정보를 설정
	req->SetURL(url);
	req->SetVerb(TEXT("POST"));
	req->SetHeader(TEXT("Content-Type"), TEXT("Application/json")); //Http Content type 
	req->SetContentAsString(json);
	//응답받을 함수를 연결
	req->OnProcessRequestComplete().BindUObject(this, &AHttpActor::OnRsqPostJoinWorld);
	//서버에 요청
	req->ProcessRequest();
}

void AHttpActor::OnRsqPostJoinWorld(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		//통신성공
		FString result = Response->GetContentAsString(); //어떤 타입으로 받을 것인지
		int32 ResultResponseCode = Response->GetResponseCode();
		//필요한 정보를 뽑아서 화면에 출력하고 싶다.
		HttpUI->RecvFindSessionInfo(result,ResultResponseCode);
	}
	else
	{
		//통신성공
		UE_LOG(LogTemp, Error, TEXT("OnResPostTest Fail.."));
	}
}
void AHttpActor::RsqPostCreateID(FString url, FString json)
{
	//Http 모듈 생성
	FHttpModule& httpModule = FHttpModule::Get();
	//IHttpRequest = TSharedRef<IHttpRequest> //스마트 포인터 //댕글링포인터가 되는것을 방지
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();

	//요청할 정보를 설정
	req->SetURL(url);
	req->SetVerb(TEXT("POST"));
	req->SetHeader(TEXT("Content-Type"), TEXT("Application/json")); //Http Content type 
	req->SetContentAsString(json);
	//응답받을 함수를 연결
	req->OnProcessRequestComplete().BindUObject(this, &AHttpActor::OnRsqPostCreateID);
	//서버에 요청
	req->ProcessRequest();
}

void AHttpActor::OnRsqPostCreateID(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		//통신성공
		FString result = Response->GetContentAsString(); //어떤 타입으로 받을 것인지
		//필요한 정보를 뽑아서 화면에 출력하고 싶다.
		int code = Response->GetResponseCode();
		LoginScreenHttpUI->OnButtonRegisterResponse(result,code);
	}
	else
	{
		//통신성공
		UE_LOG(LogTemp, Error, TEXT("OnResPostTest Fail.."));
	}
}
void AHttpActor::RsqPostLogin(FString url, FString json)
{
	//Http 모듈 생성
	FHttpModule& httpModule = FHttpModule::Get();
	//IHttpRequest = TSharedRef<IHttpRequest> //스마트 포인터 //댕글링포인터가 되는것을 방지
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();

	//요청할 정보를 설정
	req->SetURL(url);
	req->SetVerb(TEXT("POST"));
	req->SetHeader(TEXT("Content-Type"), TEXT("Application/json")); //Http Content type 
	req->SetContentAsString(json);
	//응답받을 함수를 연결
	req->OnProcessRequestComplete().BindUObject(this, &AHttpActor::OnRsqPostLogin);
	//서버에 요청
	req->ProcessRequest();
}

void AHttpActor::OnRsqPostLogin(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		//통신성공
		FString result = Response->GetContentAsString(); //어떤 타입으로 받을 것인지
		//필요한 정보를 뽑아서 화면에 출력하고 싶다.
		int code = Response->GetResponseCode();
		LoginScreenHttpUI->OnButtonLoginResponse(result,code);
	}
	else
	{
		//통신성공
		UE_LOG(LogTemp, Error, TEXT("OnResPostTest Fail.."));
	}
}
void AHttpActor::RsqGetFindSession(FString url)
{
	//Http 모듈 생성
	FHttpModule& httpModule = FHttpModule::Get();
	//IHttpRequest = TSharedRef<IHttpRequest> //스마트 포인터 //댕글링포인터가 되는것을 방지
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();

	//요청할 정보를 설정
	req->SetURL(url);
	req->SetVerb(TEXT("GET"));
	req->SetHeader(TEXT("Content-Type"), TEXT("Application/json")); //Http Content type 
	//응답받을 함수를 연결
	req->OnProcessRequestComplete().BindUObject(this, &AHttpActor::OnRsqGetFindSession);
	//서버에 요청
	req->ProcessRequest();
}


void AHttpActor::OnRsqGetFindSession(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		//통신성공
		FString result = Response->GetContentAsString(); //어떤 타입으로 받을 것인지
		int32 ResultResponseCode = Response->GetResponseCode();

		//필요한 정보를 뽑아서 화면에 출력하고 싶다.
		HttpUI->RecvFindSessionInfo(result,ResultResponseCode);
	}
	else
	{
		//통신성공
		UE_LOG(LogTemp, Error, TEXT("bConnectedSuccessfully Fail.."));
	}
}

