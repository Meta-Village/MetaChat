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
	//Http ��� ����
	FHttpModule& httpModule = FHttpModule::Get();
	//IHttpRequest = TSharedRef<IHttpRequest> //����Ʈ ������ //��۸������Ͱ� �Ǵ°��� ����
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();

	//��û�� ������ ����
	req->SetURL(url);
	req->SetVerb(TEXT("GET"));
	req->SetHeader(TEXT("Content-Type"), TEXT("Application/json")); //Http Content type 
	//������� �Լ��� ����
	req->OnProcessRequestComplete().BindUObject(this, &AHttpActor::OnRsqGetTest);
	//������ ��û
	req->ProcessRequest();
}


void AHttpActor::OnRsqGetTest(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		//��ż���
		FString result = Response->GetContentAsString(); //� Ÿ������ ���� ������


		//�ʿ��� ������ �̾Ƽ� ȭ�鿡 ����ϰ� �ʹ�.

		HttpUI->SetTextLog(UJsonParseLib::JsonParsePassword(result));

	}
	else
	{
		//��ż���
		UE_LOG(LogTemp, Error, TEXT("bConnectedSuccessfully Fail.."));
	}
}

void AHttpActor::RsqPostTest(FString url, FString json)
{
	//Http ��� ����
	FHttpModule& httpModule = FHttpModule::Get();
	//IHttpRequest = TSharedRef<IHttpRequest> //����Ʈ ������ //��۸������Ͱ� �Ǵ°��� ����
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();

	//��û�� ������ ����
	req->SetURL(url);
	req->SetVerb(TEXT("POST"));
	req->SetHeader(TEXT("Content-Type"), TEXT("Application/json")); //Http Content type 
	req->SetContentAsString(json);
	//������� �Լ��� ����
	req->OnProcessRequestComplete().BindUObject(this, &AHttpActor::OnResPostTest);
	//������ ��û
	req->ProcessRequest();
}

void AHttpActor::OnResPostTest(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		//��ż���
		FString result = Response->GetContentAsString(); //� Ÿ������ ���� ������
		//�ʿ��� ������ �̾Ƽ� ȭ�鿡 ����ϰ� �ʹ�.
		HttpUI->SetTextLog(result);
	}
	else
	{
		//��ż���
		UE_LOG(LogTemp, Error, TEXT("OnResPostTest Fail.."));
	}
}