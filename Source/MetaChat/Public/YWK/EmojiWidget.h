// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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

	UPROPERTY(meta = (BindWidget))
	class UImage* IMG_Emoji;

	UFUNCTION()
	void OnReCeiveEmoji();

	UFUNCTION(Server, Reliable)
	void ServerShowEmoji();

	UFUNCTION(NetMulticast, Reliable)
	void MultiShowEmoji();
};
