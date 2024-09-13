// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/MeetingButton.h"
#include "Components/Button.h"
#include "HSB/CustomCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "LSJ/MetaChatGameInstance.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"  // IHttpResponse 헤더 파일 포함
#include "JsonUtilities.h"
#include "Serialization/JsonSerializer.h"

void UMeetingButton::NativeConstruct()
{
	Super::NativeConstruct();
	
	meetingId = 0;
	Bt_MeetingStart->OnClicked.AddDynamic(this, &UMeetingButton::MeetingStart_Clicked);
	Bt_MeetingEnd->OnClicked.AddDynamic(this, &UMeetingButton::MeetingEnd_Clicked);
}

void UMeetingButton::MeetingStart_Clicked()
{
	// 버튼을 누르면 현재 시간을 저장하고 싶다.
	FDateTime CurrentTime = FDateTime::Now();
	// 이것을 백엔드가 요청하는 시간 타입(시작)으로 바꿔야 한다.
	FString meetStartTime = CurrentTime.ToIso8601();
	// 이것을 백엔드가 요청하는 시간 타입(끝)으로 바꿔야 하지만 빈값으로 보내버리기.
	FString meetEndTime = "";
	// 버튼을 누르면 meetingId를 int값으로 생성하고 싶다.
	this->meetingId = 0;
	// 현재 있는 방의 정보를 담아야 한다.
	FString ZoneName = "Unknown";
	ACustomCharacter* PlayerCharacter = Cast<ACustomCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter)
	{
		//ZoneName 정보 가져오기
		 ZoneName = PlayerCharacter->GetCurrentZoneName();
	}
	// 월드 아이디 가져오기
	int32 WorldId = -1; // 기본값
	auto* gi = Cast<UMetaChatGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		WorldId = gi->WorldID;
	}
	
	
	//UWorld* World = GetWorld();

	//// 현재 월드의 Id를 담아야 한다.
	//if (World)
	//{
	//	WorldId = World->GetUniqueID(); // 월드 고유 ID 가져오기
	//	UE_LOG(LogTemp, Log, TEXT("World ID : %d"), WorldId);
	//}
	// OnMeetingStart에 보낼 정보들...
	OnMeetingStart(this->meetingId, meetStartTime, meetEndTime, ZoneName, WorldId);
}

void UMeetingButton::MeetingEnd_Clicked()
{
	// 버튼을 누르면 현재 시간을 저장하고 싶다.
	FDateTime CurrentedTime = FDateTime::Now();
	// 이것또한 백엔드가 요청하는 시간 타입으로 바꾼다.
	FString meetEndTime = CurrentedTime.ToIso8601();

	// 종료 시간과 함께 PATCH요청을 보내기
	SendMeetingEnd(this->meetingId, meetEndTime);
}

void UMeetingButton::OnMeetingStart(int32 InMeetingId, const FString& meetStartTime, const FString& meetEndTime, const FString& ZoneName, int32 WorldId)
{	
	this->meetingId = InMeetingId;
	//Json형식으로 만들기
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetNumberField(TEXT("meetingId"), InMeetingId);
	JsonObject->SetStringField(TEXT("meetStartTime"),meetStartTime);
	JsonObject->SetStringField(TEXT("meetEndTime"),meetEndTime); // 시작때는 빈칸
	JsonObject->SetStringField(TEXT("ZoneName"),ZoneName);
	JsonObject->SetNumberField(TEXT("WorldId"),WorldId);

	// JSON 직렬화 (문자열로 변환)
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	// 서버로 POST 요청 전송
	SendMeetingStart(OutputString);

}

void UMeetingButton::SendMeetingStart(const FString& JsonString)
{
	//Http 모듈 초기화
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http)
	{
		UE_LOG(LogTemp, Warning, TEXT("HTTP module is not available"));
		return;
	}

	// Http 요청 생성
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	
	// 서버의 API URL을 설정
	Request->SetURL(TEXT("http://125.132.216.190:8126/api/meeting"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// 요청 본문에 json 데이터를 설정
	Request->SetContentAsString(JsonString);

	// 요청 완료 콜백을 바인딩
	Request->OnProcessRequestComplete().BindUObject(this, &UMeetingButton::OnResponseReceived);

	if (!Request->ProcessRequest())
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Send HTTP request"));
	}
}

void UMeetingButton::SendMeetingEnd(int32 InMeetingId, const FString& meetEndTime)
{
	this->meetingId = InMeetingId;
}

void UMeetingButton::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		// 서버로부터의 응답 처리
		FString ResponseContent = Response->GetContentAsString();
		UE_LOG(LogTemp, Log, TEXT("Response from server: %s"), *ResponseContent);

		// JSON 파싱이 필요한 경우 처리
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			int32 Status = JsonObject->GetIntegerField(TEXT("status"));
			FString Message = JsonObject->GetStringField(TEXT("message"));
			UE_LOG(LogTemp, Log, TEXT("Status: %d, Message: %s"), Status, *Message);
		}
	}
	else
	{
		// 요청 실패 처리
		UE_LOG(LogTemp, Error, TEXT("Failed to receive valid response from server"));
	}
}

