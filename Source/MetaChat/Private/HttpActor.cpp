// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpActor.h"
#include "UserWidgetTEST.h"
#include <HttpModule.h>
#include "JsonParseLib.h"
// Sets default values
AHttpActor::AHttpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHttpActor::BeginPlay()
{
	Super::BeginPlay();
	
	HttpUI = Cast<UUserWidgetTEST>(CreateWidget(GetWorld(), HttpUIFactory));
	if (HttpUI)
	{
		HttpUI->AddToViewport();
		HttpUI->SetHttpActor(this);
	}

}

// Called every frame
void AHttpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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


		//필요한 정보를 뽑아서 화면에 출력하고 싶다.

		HttpUI->SetTextLog(UJsonParseLib::JsonParsePassword(result));

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
		//필요한 정보를 뽑아서 화면에 출력하고 싶다.
		HttpUI->SetTextLog(result);
	}
	else
	{
		//통신성공
		UE_LOG(LogTemp, Error, TEXT("OnResPostTest Fail.."));
	}
}