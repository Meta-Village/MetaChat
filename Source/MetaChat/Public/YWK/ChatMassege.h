// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatMassege.generated.h"

/**
 * 
 */
UCLASS()
class METACHAT_API UChatMassege : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	// 텍스트 블럭(플레이어 이름)
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerName_Text;

	// 텍스트 블럭(채팅 메세지)
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ChatMessage_Text_1;

	// 플레이어 이름과 채팅 메시지를 설정하는 함수
	void SetChatData(const FString& InputPlayerName, const FString& ChatMessage);

	
};
