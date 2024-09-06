// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatPanel.generated.h"

/**
 * 
 */
UCLASS()
class METACHAT_API UChatPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UScrollBox* Chat_ScrollBox;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UEditableTextBox* ChatInputBox;

	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void UpdateChat(const FString& PlayerName, const FString& ChatMessage);

	// 채팅내역 다시 받기
	void RequestChatHistory();

private:
	void SendChatToServer(const FString& PlayerName, const FString& ChatMessage);

	void SendChatToServerHttp(const FString& PlayerName, const FString& ChatMessage);

	
};
