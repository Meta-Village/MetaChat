// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WindowScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class METACHAT_API UWindowScreenWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // 이미지를 동적으로 설정하기 위한 함수
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetImage(UTexture2D* NewImage);

protected:
    // UMG 위젯의 이미지 컴포넌트 (UMG 디자이너에서 할당한 이미지)
    UPROPERTY(meta = (BindWidget))
    class UImage* ImageWindowScreen;
};
