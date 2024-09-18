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
	UPROPERTY(meta = (BindWidget))
	class UButton* ViewButton;
	bool bClicked;
protected:
	virtual void NativeOnInitialized();
public:
	void SetUserID(FString UserID);
	UFUNCTION()
	void ViewButtonOnClick();
	FUserIDButtonDelegate UserIDButtonDelegate;
	FUserIDButtonDelegate_OneParam FUserIDButtonDelegate_OneParam;
	FUserIDButtonDelegate_TwoParams UserIDButtonDelegate_TwoParams;
};
