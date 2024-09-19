// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SharingUserSlot.generated.h"
DECLARE_DELEGATE(FUserIDButtonDelegate)
DECLARE_DELEGATE_OneParam(FUserIDButtonDelegate_OneParam, FString)
DECLARE_DELEGATE_TwoParams(FUserIDButtonDelegate_TwoParams, FString, bool)
/**
 * 
 */
UCLASS()
class METACHAT_API USharingUserSlot : public UUserWidget
{
	GENERATED_BODY()
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TextUserID;
	class UTexture2D* TextureClicked;
	class UTexture2D* TextureIdle;
	bool bClicked;
protected:
	virtual void NativeOnInitialized();
public:
	UPROPERTY(meta = (BindWidget))
	class UButton* ViewButton;
	void SetUserID(FString UserID);
	void SetButtonStyle(UButton* Button, UTexture2D* NormalTexture, UTexture2D* PressedTexture, UTexture2D* HoveredTexture);
	UFUNCTION()
	void ViewButtonOnClick();
	FUserIDButtonDelegate UserIDButtonDelegate;
	FUserIDButtonDelegate_OneParam FUserIDButtonDelegate_OneParam;
	FUserIDButtonDelegate_TwoParams UserIDButtonDelegate_TwoParams;
};
