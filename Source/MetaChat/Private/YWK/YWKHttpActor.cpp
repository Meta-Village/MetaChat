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
#include "YWK/MeetingButton.h"
#include "YWK/EmojiWidget.h"
#include "HSB/CustomCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
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
    // BeginPlay 호출 확인
    UE_LOG(LogTemp, Log, TEXT("AYWKHttpActor BeginPlay called"));

    if (!YWKHttpUI && HttpUIFactory)  // YWKHttpUI가 존재하지 않으면 생성
    {
        YWKHttpUI = Cast<UYWKHttpUI>(CreateWidget(GetWorld(), HttpUIFactory));

        if (YWKHttpUI)
        {
            YWKHttpUI->AddToViewport();
            UE_LOG(LogTemp, Log, TEXT("YWKHttpUI successfully created and added to viewport."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create YWKHttpUI widget."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("YWKHttpUI already exists, skipping creation."));
    }

    // 미팅버튼
    if (!MeetingUI && MeetingUIFactory)  // MeetingUI가 존재하지 않으면 생성
    {
        MeetingUI = Cast<UMeetingButton>(CreateWidget(GetWorld(), MeetingUIFactory));

        if (MeetingUI)
        {
            MeetingUI->AddToViewport();
            UE_LOG(LogTemp, Log, TEXT("MeetingUI successfully created and added to viewport."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create MeetingUI widget."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("MeetingUI already exists, skipping creation."));
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
        // 서버 응답 헤더에서 Content-Type을 확인
        FString ContentType = Response->GetHeader(TEXT("Content-Type"));

        if (ContentType.Contains(TEXT("image/png")) || ContentType.Contains(TEXT("image/jpeg")))
        {
            // 이미지 데이터를 직접 처리 (이모티콘 이미지 응답)
            TArray<uint8> ImageData = Response->GetContent();
            UE_LOG(LogTemp, Log, TEXT("Image data received from server, size: %d"), ImageData.Num());

            // 캐릭터의 EmojiWidget을 찾아서 이미지 처리
            ACustomCharacter* Character = Cast<ACustomCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
            if (Character && Character->EmojiUIComp)
            {
                UEmojiWidget* EmojiWidget = Cast<UEmojiWidget>(Character->EmojiUIComp->GetUserWidgetObject());
                if (EmojiWidget)
                {
                    // 이미지를 EmojiWidget에 표시
                    EmojiWidget->SetEmojiImageFromData(ImageData);
                    UE_LOG(LogTemp, Log, TEXT("Emoji image successfully passed to EmojiWidget."));
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Failed to find UEmojiWidget in Character's EmojiUIComp."));
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to find Character or EmojiUIComp."));
            }
        }
        else
        {
            // JSON 데이터를 처리
            FString ResponseString = Response->GetContentAsString();
            UE_LOG(LogTemp, Log, TEXT("Response from server: %s"), *ResponseString);

            TSharedPtr<FJsonObject> JsonObject;
            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
            if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
            {
                // 메시지 요약 처리
                if (JsonObject->HasField("messages"))
                {
                    FString MeetingSummary = JsonObject->GetStringField("messages");
                    UE_LOG(LogTemp, Log, TEXT("Meeting summary: %s"), *MeetingSummary);

                    // YWKHttpUI가 null인 경우 재생성
                    if (!YWKHttpUI)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("YWKHttpUI is null, trying to create it again."));
                        if (HttpUIFactory)
                        {
                            YWKHttpUI = Cast<UYWKHttpUI>(CreateWidget(GetWorld(), HttpUIFactory));
                            if (YWKHttpUI)
                            {
                                YWKHttpUI->AddToViewport();
                                UE_LOG(LogTemp, Log, TEXT("YWKHttpUI successfully recreated and added to viewport."));
                            }
                            else
                            {
                                UE_LOG(LogTemp, Error, TEXT("Failed to recreate YWKHttpUI widget."));
                            }
                        }
                    }

                    // YWKHttpUI가 유효하면 미팅 로그 업데이트
                    if (YWKHttpUI)
                    {
                        YWKHttpUI->SetTextLog(MeetingSummary);
                    }
                }

                // 이모티콘 파일명 처리
                if (JsonObject->HasField("emojiFileName"))
                {
                    FString EmojiFileName = JsonObject->GetStringField("emojiFileName");
                    UE_LOG(LogTemp, Log, TEXT("Emoji file name received: %s"), *EmojiFileName);

                    // 이모티콘을 받을 URL로 이미지 요청
                    FString EmojiUrl = FString::Printf(TEXT("https://eagle-prepared-octopus.ngrok-free.app/chatbot/emote?filename=%s"), *EmojiFileName);

                    // GET 요청을 통해 이모티콘 이미지 다운로드
                    FHttpModule* Http = &FHttpModule::Get();
                    if (Http)
                    {
                        TSharedRef<IHttpRequest, ESPMode::ThreadSafe> GetRequest = Http->CreateRequest();
                        GetRequest->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
                        {
                            if (bWasSuccessful && Response->GetContentLength() > 0)
                            {
                                // 서버로부터 받은 이미지 데이터를 처리
                                TArray<uint8> ImageData = Response->GetContent();

                                // 캐릭터의 EmojiWidget을 찾아서 이미지 처리
                                ACustomCharacter* Character = Cast<ACustomCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
                                if (Character && Character->EmojiUIComp)
                                {
                                    UEmojiWidget* EmojiWidget = Cast<UEmojiWidget>(Character->EmojiUIComp->GetUserWidgetObject());
                                    if (EmojiWidget)
                                    {
                                        // 이미지를 EmojiWidget에 표시
                                        EmojiWidget->SetEmojiImageFromData(ImageData);
                                        UE_LOG(LogTemp, Log, TEXT("Emoji image successfully passed to EmojiWidget."));
                                    }
                                    else
                                    {
                                        UE_LOG(LogTemp, Error, TEXT("Failed to find UEmojiWidget in Character's EmojiUIComp."));
                                    }
                                }
                                else
                                {
                                    UE_LOG(LogTemp, Error, TEXT("Failed to find Character or EmojiUIComp."));
                                }
                            }
                            else
                            {
                                UE_LOG(LogTemp, Error, TEXT("Failed to download emoji image."));
                            }
                        });

                        GetRequest->SetURL(EmojiUrl);
                        GetRequest->SetVerb("GET");
                        GetRequest->ProcessRequest();
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("No emojiFileName found in server response"));
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to parse server response: %s"), *ResponseString);
            }
        }
    }
    else
    {
        // 요청이 실패한 경우
        if (Response.IsValid())
        {
            FString ErrorMessage = Response->GetContentAsString();
            UE_LOG(LogTemp, Error, TEXT("Server response error: %s"), *ErrorMessage);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to connect to server or invalid response"));
        }
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
    FString MeetingIdPart = FString::Printf(TEXT("--%s\r\nContent-Disposition: form-data; name=\"meetingId\"\r\n\r\n%d\r\n"), *Boundary, NewMeetingID);

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

void AYWKHttpActor::SendEmojiRequest(const FString& ChatMessage)
{
    // HTTP 모듈 생성
    FHttpModule& HttpModule = FHttpModule::Get();

    // Json 형식으로 서버에 보낼 데이터 생성
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    JsonObject->SetStringField(TEXT("minutes"), ChatMessage);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    // HTTP 요청 설정
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule.CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &AYWKHttpActor::OnEmojiResponseReceived);
    Request->SetURL(TEXT("https://eagle-prepared-octopus.ngrok-free.app/chatbot/emote"));  // FastAPI 라우트 URL
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetContentAsString(OutputString);
    Request->ProcessRequest();
}

void AYWKHttpActor::OnEmojiResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
    if (bConnectedSuccessfully && Response.IsValid())
    {
        // 서버 응답 내용 확인
        FString ResponseString = Response->GetContentAsString();
        UE_LOG(LogTemp, Log, TEXT("Emoji response received from server: %s"), *ResponseString);

        // 응답이 성공했는지 여부 확인 (예: 상태 코드가 200인지 확인)
        int32 ResponseCode = Response->GetResponseCode();
        if (ResponseCode == 200)  // 200 OK 응답일 경우 처리
        {
            UE_LOG(LogTemp, Log, TEXT("Emoji request successfully processed by server"));

            // 서버에서 반환된 이미지 URL을 받아서 처리
            if (UChatPanel* ChatPanel = Cast<UChatPanel>(FindObject<UChatPanel>(ANY_PACKAGE, TEXT("ChatPanel"))))
            {
                ChatPanel->ReceiveToServerEmoji(ResponseString);  // 이모티콘 URL을 전송
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Server responded but with error code: %d"), ResponseCode);
        }
    }
    else
    {
        // 통신 실패
        UE_LOG(LogTemp, Error, TEXT("Failed to receive response from server"));
    }
}
