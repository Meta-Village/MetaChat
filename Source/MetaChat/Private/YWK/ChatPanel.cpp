// Fill out your copyright notice in the Description page of Project Settings.



#include "YWK/ChatPanel.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "Serialization/JsonWriter.h"
#include "YWK/YWKHttpActor.h"
#include "LSJ/MetaChatGameInstance.h"
#include "HSB/CustomCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/IHttpRequest.h"

void UChatPanel::NativeConstruct()
{
	Super::NativeConstruct();
	// 텍스트 입력할 때 엔터키 눌렀을 때 이벤트 처리
	if (ChatInputBox)
	{
		ChatInputBox->OnTextCommitted.AddDynamic(this, &UChatPanel::OnTextCommitted);
	}
    RequestChatHistory();
}

void UChatPanel::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    if (CommitMethod == ETextCommit::OnEnter)
    {
        FString ChatMessage = Text.ToString();

        // GameInstance에서 UserID 가져오기
        auto* gi = Cast<UMetaChatGameInstance>(GetWorld()->GetGameInstance());
        if (!gi)
        {
            UE_LOG(LogTemp, Warning, TEXT("GameInstance is null, cannot send chat"));
            return;
        }

        // UserID를 PlayerName으로 사용
        FString PlayerName = gi->UserID;

        // **디버깅 로그로 UserID 확인**
        UE_LOG(LogTemp, Log, TEXT("PlayerName (UserID) is: %s"), *PlayerName);

        // 서버로 채팅 전송(로컬)
        SendChatToServer(PlayerName, ChatMessage);

        // 서버로 채팅 전송(서버)
        SendChatToServerHttp(PlayerName, ChatMessage);

        // 채팅 입력 필드 초기화
        if (ChatInputBox)
        {
            ChatInputBox->SetText(FText::GetEmpty());
        }
    }
}

void UChatPanel::SendChatToServer(const FString& PlayerName, const FString& ChatMessage)
{
    APlayerController* PlayerController = Cast<APlayerController>(GetOwningPlayer());
}

void UChatPanel::UpdateChat(const FString& PlayerName, const FString& ChatMessage)
{
    UE_LOG(LogTemp, Log, TEXT("Updating chat with PlayerName: %s and ChatMessage: %s"), *PlayerName, *ChatMessage);
    if (Chat_ScrollBox)
    {
        // 새로운 채팅 메세지를 위한 TextBlock 생성
        UTextBlock* NewChatMessage = NewObject<UTextBlock>(Chat_ScrollBox);
        if (NewChatMessage)
        {
            // 메시지 형식 설정
            FString FormattedMessage = FString::Printf(TEXT("%s: %s"), *PlayerName, *ChatMessage);
            NewChatMessage->SetText(FText::FromString(FormattedMessage));

            // ScrollBox에 메세지 추가
            Chat_ScrollBox->AddChild(NewChatMessage);

            // 새로운 메시지가 추가되면 스크롤을 끝으로 이동
            Chat_ScrollBox->ScrollToEnd();
        }
    }
}


void UChatPanel::SendChatToServerHttp(const FString& PlayerName, const FString& ChatMessage)
{
    // 아이디 월드 아이디 가져오기
    auto* gi = Cast<UMetaChatGameInstance>(GetWorld()->GetGameInstance());
    if (!gi)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameInstance is null, cannot send chat"));
        return;
    }
    int32 WorldId = gi->WorldID;
    FString UserId = gi->UserID;
    FString UserName = gi->UserID;
    // ZoneName을 초기화
    FString ZoneName = TEXT("");
    ACustomCharacter* PlayerCharacter = Cast<ACustomCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
    if (PlayerCharacter)
    {
        ZoneName = PlayerCharacter->GetCurrentZoneName().ToUpper();
    }
    else
    {
        ZoneName = TEXT("UNKNOWN");
    }

    // Json 형식 만들기
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

    // 서버가 필요로 하는 데이터 Json으로 채우기
    JsonObject->SetNumberField(TEXT("worldId"), WorldId); // 월드 아이디도 나중에 변경예정
    JsonObject->SetStringField(TEXT("userId"), UserId); // 나중에 변경예정
    JsonObject->SetNumberField(TEXT("meetingId"), 0); // 0으로 해야겠다
    JsonObject->SetStringField(TEXT("userName"), UserName);
    JsonObject->SetStringField(TEXT("zoneName"), ZoneName); //나중에 변경 예정
    JsonObject->SetStringField(TEXT("chatTime"), FDateTime::Now().ToIso8601()); // 현재 시간을 ISO8601 형식으로
    JsonObject->SetStringField(TEXT("chatContent"), ChatMessage);

    //Json 객체를 문자열로 변환
    FString OutputString;
    TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    // HTTP 액터를 사용해서 서버로 JSON 데이터를 보내기
    AYWKHttpActor* HttpActor = GetWorld()->SpawnActor<AYWKHttpActor>(); // HTTP 액터를 생성하거나 찾아서 사용
    if (HttpActor)
    {
        HttpActor->RsqPostTest(TEXT("http://125.132.216.190:8126/api/chat"), OutputString);
    }
}
void UChatPanel::RequestChatHistory()
{
    //GameInstance에서 WorldID와 userID가져오기 
    auto* gi = Cast<UMetaChatGameInstance>(GetWorld()->GetGameInstance());
    if (!gi)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameInstance is null, cannot request chat history"));
        return;
    }
    int32 WorldId = gi->WorldID;
    FString UserId = gi->UserID;

    //URL 구성
    FString RequestUrl = FString::Printf(TEXT("http://125.132.216.190:8126/api/chat/%d?userId=%s"),WorldId, *UserId);


    // HTTP 액터를 통해 채팅 기록 요청
    AYWKHttpActor* HttpActor = GetWorld()->SpawnActor<AYWKHttpActor>();
    if (HttpActor)
    {
        // 채팅 내역을 요청하는 GET 요청을 보낸다.
        HttpActor->RsqGetTest(RequestUrl);
    }
}

void UChatPanel::OnChatHistoryReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        FString ResponseContent = Response->GetContentAsString();
        UE_LOG(LogTemp, Log, TEXT("Chat history response: %s"), *ResponseContent);

        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

        // Json 데이터 파싱
        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            int32 status = JsonObject->GetIntegerField(TEXT("status"));
            if (status == 200)
            {
                const TArray<TSharedPtr<FJsonValue>>* ChatMessages;
                if (JsonObject->TryGetArrayField(TEXT("data"), ChatMessages))
                {
                    // 기존 채팅 기록 초기화
                    Chat_ScrollBox->ClearChildren();

                    // 채팅 메시지들을 바로 ScrollBox에 추가
                    for (const TSharedPtr<FJsonValue>& MessageValue : *ChatMessages)
                    {
                        TSharedPtr<FJsonObject> MessageObject = MessageValue->AsObject();
                        if (MessageObject.IsValid())
                        {
                            // 필요한 필드들 가져오기 (userName, chatContent)
                            FString UserName = MessageObject->GetStringField(TEXT("userName"));
                            FString ChatContent = MessageObject->GetStringField(TEXT("chatContent"));

                            // ScrollBox에 추가
                            UpdateChat(UserName, ChatContent);
                        }
                    }

                    // 새로운 메시지가 추가되면 스크롤을 끝으로 이동
                    Chat_ScrollBox->ScrollToEnd();
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to retrieve chat history. Status: %d"), status);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get chat history from server"));
    }
}



