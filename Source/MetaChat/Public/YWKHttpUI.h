// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "YWKHttpUI.generated.h"

/**
 * 
 */
UCLASS()
class METACHAT_API UYWKHttpUI : public UUserWidget
{
	GENERATED_BODY()

public:	
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnSendButtonClicked();

	void SetTextLog(const FString& NewLog);
	
	//텍스트 업데이트할 TextBlock
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UTextBlock* LogCheckBox;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UButton* SendButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UTextBlock* SummaryTextBlock;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UScrollBox* ScrollBox;

	// 타이머 핸들 선언
	FTimerHandle BlinkTimerHandle;

	// 깜빡임 상태를 저장하는 변수
	bool bIsTextVisible = true;

	//깜빡임을 제어하는 함수
	void ToggleBlinkingText();

	// 요약본 불러올 텍스트창
	void UpdateSummaryText(const FString& SummaryText);

	void UpdateWidgetTexture();
};
