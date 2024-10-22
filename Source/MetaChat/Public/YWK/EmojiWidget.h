﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "EmojiWidget.generated.h"

/**
 * 
 */
UCLASS()
class METACHAT_API UEmojiWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// 이모티콘 이미지를 숨기는 함수
	void HideEmoji();

	// 타이머 핸들 추가하기
	FTimerHandle HideEmojiTimerHandle;

	// 이모티콘 받을 이미지칸
	UPROPERTY(meta = (BindWidget))
	class UImage* IMG_Emoji;

	// 이모티콘 빌보드하기
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UBillboardComponent* EmojiBillboard;

	    // 서버로 이모티콘 요청을 보낼 함수
    void SendEmojiRequestToServer(const FString& ChatMessage);

    // 서버 응답을 처리하는 함수
    void OnEmojiResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    // url에서 이미지 로드하는 함수
    void LoadEmojiFromUrl(const FString& ImageUrl);

    // 이미지 다운로드 완료 후 호출될 함수
    void OnImageDownloaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    // 서버에서 호출될 함수
    UFUNCTION(Server, Reliable)
    void ServerShowEmoji(const FString& Filename);

	UFUNCTION()
	void SetEmojiImageFromData(const TArray<uint8>& ImageData);

	// 이미지를 TArray<uint8>에서 UTexture2D로 변환하는 함수
	UTexture2D* CreateTextureFromImageData(const TArray<uint8>& ImageData);


};
