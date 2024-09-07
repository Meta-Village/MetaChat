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
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LogCheckBox;

	UPROPERTY(meta = (BindWidget))
	class UButton* SendButton;

	void UpdateWidgetTexture();
};
