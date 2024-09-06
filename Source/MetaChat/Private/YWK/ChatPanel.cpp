// Fill out your copyright notice in the Description page of Project Settings.



#include "YWK/ChatPanel.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "Serialization/JsonWriter.h"
#include "YWK/YWKHttpActor.h"

void UChatPanel::NativeConstruct()
{
	Super::NativeConstruct();
	// 텍스트 입력할 때 엔터키 눌렀을 때 이벤트 처리
	if (ChatInputBox)
	{
		ChatInputBox->OnTextCommitted.AddDynamic(this, &UChatPanel::OnTextCommitted);
	}

}

void UChatPanel::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    if (CommitMethod == ETextCommit::OnEnter)
    {
        FString ChatMessage = Text.ToString();

        // 현재 플레이어 이름 가져오기
        APlayerController* PlayerController = Cast<APlayerController>(GetOwningPlayer());
        if (PlayerController)
        {
            // PlayerState를 가져와서 캐스팅합니다.
            APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>();
            if (PlayerState)
            {
                FString PlayerName = PlayerState->GetPlayerName();

                // 서버로 채팅 전송(로컬)
                SendChatToServer(PlayerName, ChatMessage);
                // 서버로 채팅 전송(서버)
                SendChatToServerHttp(PlayerName, ChatMessage);
                

                // 채팅 입력 필드 초기화
                ChatInputBox->SetText(FText::GetEmpty());
            }
        }
    }
}



void UChatPanel::SendChatToServer(const FString& PlayerName, const FString& ChatMessage)
{
    // PlayerController 가져오기
    APlayerController* PlayerController = Cast<APlayerController>(GetOwningPlayer());

    //if (PlayerController)
    //{
    //    // BP_TopDownPlayerController로 캐스팅
    //    if (ABP_TopDownPlayerController* BPController = Cast<ABP_TopDownPlayerController>(PlayerController))
    //    {
    //        // 블루프린트에서 정의된 SR_SubmitChat 호출
    //        BPController->SR_SubmitChat(PlayerName, ChatMessage);
    //    }
    //}
}

void UChatPanel::UpdateChat(const FString& PlayerName, const FString& ChatMessage)
{
    if (Chat_ScrollBox)
    {
        // 새로운 채팅 메세지를 위한 TextBlock 생성
        UTextBlock* NewChatMessage = NewObject<UTextBlock>(Chat_ScrollBox);
        if (NewChatMessage)
        {
            // 메시지 형식 설정
            FString FormattedMessage = FString::Printf(TEXT("%s: %s"), *PlayerName, *ChatMessage);

            // 텍스트 설정
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
    // Json 형식 만들기
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

    // 서버가 필요로 하는 데이터 Json으로 채우기
    JsonObject->SetNumberField(TEXT("WorldId"), 0);
    JsonObject->SetStringField(TEXT("userId"), "example_user_id"); // 나중에 변경
    JsonObject->SetNumberField(TEXT("meetingId"), 0);
    JsonObject->SetStringField(("userName"), PlayerName);
    JsonObject->SetStringField(("zoneName"), "ROOM1"); //나중에 변경
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
    // HTTP 액터를 통해 채팅 기록 요청
    AYWKHttpActor* HttpActor = GetWorld()->SpawnActor<AYWKHttpActor>();
    if (HttpActor)
    {
        // 채팅 내역을 요청하는 GET 요청을 보낸다.
        HttpActor->RsqGetTest(TEXT("http://125.132.216.190:8126/api/chat/{worldId}"));
    }
}
