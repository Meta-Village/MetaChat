﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LSJMainWidget.generated.h"



/**
 * 
 */
UCLASS()
class METACHAT_API ULSJMainWidget : public UUserWidget
{
	GENERATED_BODY()

	//SharingUser 버튼 패널
	UPROPERTY(meta=(BindWidget))
	class UUniformGridPanel* SharingUserPanel;
		// TSubClass
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> SharingUserSlotFactory;
	UPROPERTY()
	class USharingUserSlot* SharingUserSlot;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* ButtonWindowScreen;
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UButton* ButtonLookSharingScreen;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextWindowScreen;
		UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextLookSharingScreen;
	 UPROPERTY(meta = (BindWidget))
    class UImage* ImageSharingScreen;
	class AScreenActor* ScreenActor;
	UPROPERTY(meta = (BindWidget))
	class UImage* ImageCoveringScreen;
	bool bStreaming;
	bool bLookStreaming;
	UFUNCTION()
	void SetUserID(FString ID);
protected:
	virtual void NativeOnInitialized();
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);
private:
	// 버튼을 눌렀을 때, 호출될 델리게이트에 등록할 함수
	UFUNCTION(BlueprintCallable)
	void OnButtonWindowScreen();
	UFUNCTION(BlueprintCallable)
	void OnButtonLookSharingScreen();
public:
	void SetScreenActor(class AScreenActor* Actor);
	FString GetCurrentSessionID();
	//SharingUser 버튼 초기화

	void InitSlot(TArray<FString> Items);

};
