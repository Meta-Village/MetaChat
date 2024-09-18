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

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	// 위젯 스윗쳐
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher_50;

	// 스윗쳐 보이기
	UFUNCTION()
	void VisibleSwitcher(bool bIsVisible);

};
