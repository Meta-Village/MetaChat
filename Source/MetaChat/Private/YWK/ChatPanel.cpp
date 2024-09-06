// Fill out your copyright notice in the Description page of Project Settings.



#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "YWK/ChatPanel.h"

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

                // 서버로 채팅 전송
                SendChatToServer(PlayerName, ChatMessage);

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

}
