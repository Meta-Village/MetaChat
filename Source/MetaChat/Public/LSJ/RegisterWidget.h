// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RegisterWidget.generated.h"

/**
 * 
 */
UCLASS()
class METACHAT_API URegisterWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonRegisterClose;
	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonRegistration;
	UPROPERTY(meta=(BindWidget))
	class UEditableText* EditID;
	UPROPERTY(meta=(BindWidget))
	class UEditableText* EditPW;
};
