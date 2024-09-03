// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RecordButton.generated.h"

/**
 * 
 */
UCLASS()
class METACHAT_API URecordButton : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// 재생버튼
	UFUNCTION()
	void OnPlayButtonClicked();

	// 스탑버튼
	UFUNCTION()
	void OnStopButtonClicked();
	
	// 녹음 시작기능 호출
	void StartRecording();

	// 녹음 중단기능 호출
	void StopRecording();

	//UI들에 대한 참조
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UTextBlock* TextBlock_96;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UWidgetSwitcher* WidgetSwitcher_50;
};
