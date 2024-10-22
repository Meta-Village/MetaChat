﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/ChatMassege.h"
#include "Components/TextBlock.h"
#include "LSJ/MetaChatGameInstance.h"


void UChatMassege::NativeConstruct()
{
	Super::NativeConstruct();
}
// 채팅 칠 때 닉네임과 내용 
void UChatMassege::SetChatData(const FString& InputPlayerName, const FString& ChatMessage)
{
	auto* gi = Cast<UMetaChatGameInstance>(GetWorld()->GetGameInstance());
	if (!gi)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance is null, cannot send chat"));
		return;
	}

    // 플레이어네임과 유저아이디 동일하게 
	FString PlayerName = gi->UserID;

    
    if (PlayerName_Text)
    {
        PlayerName_Text->SetText(FText::FromString(InputPlayerName));
        UE_LOG(LogTemp, Log, TEXT("PlayerName set in widget: %s"), *InputPlayerName);
    }

    if (ChatMessage_Text_1)
    {
        ChatMessage_Text_1->SetText(FText::FromString(ChatMessage));
        UE_LOG(LogTemp, Log, TEXT("ChatMessage set in widget: %s"), *ChatMessage);
    }
}

