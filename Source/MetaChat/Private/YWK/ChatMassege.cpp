// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/ChatMassege.h"
#include "Components/TextBlock.h"


void UChatMassege::NativeConstruct()
{
	Super::NativeConstruct();
}

void UChatMassege::SetChatData(const FString& PlayerName, const FString& ChatMessage)
{
	if (PlayerName_Text)
	{
		PlayerName_Text->SetText(FText::FromString(PlayerName));
	}
	if (ChatMessage_Text_1)
	{
		ChatMessage_Text_1->SetText(FText::FromString(ChatMessage));
	}
}
