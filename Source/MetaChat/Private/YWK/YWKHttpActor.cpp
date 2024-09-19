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
			UE_LOG(LogTemp, Log, TEXT("YWKHttpUI successfully created and added to viewport."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create HttpUI widget."));
		}
	}
	// 채팅 내역을 요청하기 전에 조건을 확인(플레이어가 존재하는지, worldId가 유효한지)
	APlayerController* PlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController)
	{
		// 예: 플레이어가 준비되었을 때 채팅 내역 요청
		UChatPanel* ChatPanel = FindObject<UChatPanel>(GetWorld(), TEXT("ChatPanel"));
		if (ChatPanel)
		{
			ChatPanel->RequestChatHistory();
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
		// 통신 성공
		FString result = Response->GetContentAsString(); // 응답 데이터를 문자열로 받음

		// 1. 서버 응답 데이터를 로그로 출력
		UE_LOG(LogTemp, Log, TEXT("Response from server: %s"), *result);

		// 2. 파싱 로직은 응답 데이터를 확인한 후 추가
		/*
		if (YWKHttpUI)
		{
			YWKHttpUI->SetTextLog(UYWKJsonParseLib::JsonParsePassword(result));
		}
		*/
	}
	else
	{
		// 통신 실패
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
			// 1. 여기는 서버 응답에서 'chatMessages' 배열을 가져옴(방 나갔다 들어올 때 채팅내역 받는 부분)
			if (JsonObject->HasField("chatMessages"))
			{
				TArray<TSharedPtr<FJsonValue>> ChatArray = JsonObject->GetArrayField("chatMessages");

				// 채팅 메시지가 없을 경우 처리
				if (ChatArray.Num() == 0)
				{
					UE_LOG(LogTemp, Warning, TEXT("No chat messages found."));
					return; // 채팅 메시지가 없으면 리턴
				}

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
				UE_LOG(LogTemp, Warning, TEXT("chatMessages field not found."));
			}
			// 2. 회의 요약 가져오기
			if (JsonObject->HasField("messages") && !JsonObject->GetField<EJson::None>("messages")->IsNull())
			{
				FString MeetingSummary = JsonObject->GetStringField("messages");
				UE_LOG(LogTemp, Log, TEXT("Meeting summary: %s"), *MeetingSummary);

				//UI 업데이트 - YWKHttpUI에서 SetTextLog를 호출하여 텍스트 설정
				if (YWKHttpUI)
				{
					YWKHttpUI->SetTextLog(MeetingSummary); // 회의 요약을 UI의 TextBlock에서 처리
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("messages field not found or null"));
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

void AYWKHttpActor::RsqPostwavfile(FString url, FString FilePath, FString MeetingId)
{
    // WAV 파일을 바이너리로 읽기
    TArray<uint8> wavFileData;
    if (!LoadWavFileToBinary(FilePath, wavFileData))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load WAV file: %s"), *FilePath);
        return;
    }

    // Http 모듈 생성
    FHttpModule& httpModule = FHttpModule::Get();
    TSharedRef<IHttpRequest> req = httpModule.CreateRequest();

    // 요청할 정보를 설정
    req->SetURL(url);
    req->SetVerb(TEXT("POST"));

    // multipart/form-data 포맷으로 전송
    req->SetHeader(TEXT("Content-Type"), TEXT("multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW"));

    // Boundary와 파일 이름 설정
    FString Boundary = TEXT("----WebKitFormBoundary7MA4YWxkTrZu0gW");
    FString FileName = FPaths::GetCleanFilename(FilePath);

    // FormData에 추가할 각 파트를 작성
    // 1. meetingId 파트 (텍스트)
    FString MeetingIdPart = FString::Printf(TEXT("--%s\r\nContent-Disposition: form-data; name=\"meetingId\"\r\n\r\n%s\r\n"), *Boundary, *MeetingId);

    // 2. voice 파트 (파일)
    FString FilePartHeader = FString::Printf(
        TEXT("--%s\r\nContent-Disposition: form-data; name=\"voice\"; filename=\"%s\"\r\nContent-Type: audio/wav\r\n\r\n"), 
        *Boundary, *FileName);

    // 3. FormData를 구성
    TArray<uint8> FormData;

    // 텍스트 파트 (meetingId)를 UTF-8로 변환하고 추가
    FTCHARToUTF8 MeetingIdConverter(*MeetingIdPart);
    FormData.Append(reinterpret_cast<const uint8*>(MeetingIdConverter.Get()), MeetingIdConverter.Length());

    // 파일 헤더 파트를 UTF-8로 변환하고 추가
    FTCHARToUTF8 FileHeaderConverter(*FilePartHeader);
    FormData.Append(reinterpret_cast<const uint8*>(FileHeaderConverter.Get()), FileHeaderConverter.Length());

    // 파일 바이너리 데이터를 추가
    FormData.Append(wavFileData);

    // 폼 데이터 끝에 boundary 추가
    FString EndBoundary = FString::Printf(TEXT("\r\n--%s--\r\n"), *Boundary);
    FTCHARToUTF8 EndBoundaryConverter(*EndBoundary);
    FormData.Append(reinterpret_cast<const uint8*>(EndBoundaryConverter.Get()), EndBoundaryConverter.Length());

    // 최종 FormData를 본문에 설정
    req->SetContent(FormData);

    // 응답 받을 함수를 연결
    req->OnProcessRequestComplete().BindUObject(this, &AYWKHttpActor::OnResPostTest);

    // 서버에 요청 전송
    req->ProcessRequest();
}