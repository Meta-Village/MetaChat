// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/YWKHttpActor.h"
#include "LSJ/UserWidgetTEST.h"
#include <HttpModule.h>
#include "YWK/YWKJsonParseLib.h"

#include "Engine/Texture2D.h"
#include "Engine/Texture2DDynamic.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#include <vector> 
#include "YWKHttpUI.h"
#include "YWK/ChatPanel.h"
// Sets default values
AYWKHttpActor::AYWKHttpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AYWKHttpActor::BeginPlay()
{
	Super::BeginPlay();

	if (!YWKHttpUI && HttpUIFactory)
	{
		YWKHttpUI = Cast<UYWKHttpUI>(CreateWidget(GetWorld(), HttpUIFactory));
		if (YWKHttpUI)
		{
			YWKHttpUI->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create HttpUI widget."));
		}
	}
}

// Called every frame
void AYWKHttpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(YWKHttpUI)
		YWKHttpUI->UpdateWidgetTexture();
}

void AYWKHttpActor::RsqGetTest(FString url)
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
	req->OnProcessRequestComplete().BindUObject(this, &AYWKHttpActor::OnRsqGetTest);
	//서버에 요청
	req->ProcessRequest();
}


void AYWKHttpActor::OnRsqGetTest(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		//통신성공
		FString result = Response->GetContentAsString(); //어떤 타입으로 받을 것인지


		//필요한 정보를 뽑아서 화면에 출력하고 싶다.

		YWKHttpUI->SetTextLog(UYWKJsonParseLib::JsonParsePassword(result));

	}
	else
	{
		//통신성공
		UE_LOG(LogTemp, Error, TEXT("bConnectedSuccessfully Fail.."));
	}
}

void AYWKHttpActor::RsqPostTest(FString url, FString json)
{
	//Http 모듈 생성
	FHttpModule& httpModule = FHttpModule::Get();
	//IHttpRequest = TSharedRef<IHttpRequest> //스마트 포인터 //댕글링포인터가 되는것을 방지
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();

	//요청할 정보를 설정
	req->SetURL(url);
	req->SetVerb(TEXT("POST"));
	req->SetHeader(TEXT("Content-Type"), TEXT("application/json")); //Http Content type 
	req->SetContentAsString(json);
	//응답받을 함수를 연결
	req->OnProcessRequestComplete().BindUObject(this, &AYWKHttpActor::OnResPostTest);
	//서버에 요청
	req->ProcessRequest();
}

void AYWKHttpActor::OnResPostTest(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully && Response.IsValid())
	{
		FString ResponseString = Response->GetContentAsString(); // 서버로부터 받은 응답

		UE_LOG(LogTemp, Log, TEXT("Response from server: %s"), *ResponseString);

		// JSON 파싱
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			// 응답 데이터에서 'chatMessages' 배열을 가져옴
			TArray<TSharedPtr<FJsonValue>> ChatArray = JsonObject->GetArrayField("chatMessages");

			// 배열을 순회하면서 각 메시지를 ChatPanel에 추가
			for (TSharedPtr<FJsonValue> Value : ChatArray)
			{
				TSharedPtr<FJsonObject> ChatObject = Value->AsObject();
				FString PlayerName = ChatObject->GetStringField("userName");
				FString ChatMessage = ChatObject->GetStringField("chatContent");

				// ChatPanel에 채팅 메시지를 업데이트
				UChatPanel* ChatPanel = FindObject<UChatPanel>(GetWorld(), TEXT("ChatPanel")); // ChatPanel을 찾아서
				if (ChatPanel)
				{
					ChatPanel->UpdateChat(PlayerName, ChatMessage); // 채팅창에 메시지 업데이트
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("OnResPostTest Fail.."));
	}
}

bool AYWKHttpActor::LoadWavFileToBinary(const FString& FilePath, TArray<uint8>& OutBinaryData)
{
	// 파일을 바이너리 형식으로 읽고 싶을 때
	if (FFileHelper::LoadFileToArray(OutBinaryData, *FilePath))
	{
		UE_LOG(LogTemp, Log, TEXT("WAV file loaded successfully : %s"), *FilePath)
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Faile to loaded successfully : %s"), *FilePath)
		return false;
	}
}

void AYWKHttpActor::RsqPostwavfile(FString url, FString FilePath)
{

	// WAV 파일을 바이너리로 읽기
	TArray<uint8> wavFileData;
	if (!LoadWavFileToBinary(FilePath, wavFileData))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load WAV file: %s"), *FilePath);
		return;
	}

	//Http 모듈 생성
	FHttpModule& httpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();

	// 요청할 정보를 설정
	req->SetURL(url);
	req->SetVerb(TEXT("POST"));
	//req->SetHeader(TEXT("Content-Type"), TEXT("audio/wav")); // Content-Type을 audio/wav로 설정
	// multipart/form data으로 전송
	req->SetHeader(TEXT("Content-Type"), TEXT("multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW"));

	// 바이너리 데이터를 요청 본문에 설정
	//req->SetContent(wavFileData);
	// multipart/form-data 포맷으로 파일 내용 구성
	FString Boundary = TEXT("----WebKitFormBoundary7MA4YWxkTrZu0gW");
	FString FileName = TEXT("test.wav");

	FString Payload = FString::Printf(TEXT("--%s\r\nContent-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\nContent-Type: audio/wav\r\n\r\n"),
	*Boundary, *FileName);

	TArray<uint8> FormData;
	// 텍스트 데이터를 UTF-8로 변환
	FTCHARToUTF8 Converter(*Payload);
	FormData.Append(reinterpret_cast<const uint8*>(Converter.Get()), (Converter.Length()));

	// 파일 바이너리 데이터 추가
	FormData.Append(wavFileData);

	//폼 데이터 끝에 boundary 추가
	FString EndBoundary = FString::Printf(TEXT("\r\n--%s--\r\n"), *Boundary);
	FTCHARToUTF8 EndConverter(*EndBoundary);
	FormData.Append(reinterpret_cast<const uint8*>(EndConverter.Get()), (EndConverter.Length()));

	//최종 FormData를 본문에 설정
	req->SetContent(FormData);

	//응답받을 함수를 연결
	req->OnProcessRequestComplete().BindUObject(this, &AYWKHttpActor::OnResPostTest);

	// 서버에 요청
	req->ProcessRequest();

}