// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class METACHAT_API ULoginScreenWidget : public UUserWidget
{
	GENERATED_BODY()
	UPROPERTY(meta=(BindWidget))
	class ULoginWidget* WBP_Login;
	UPROPERTY(meta=(BindWidget))
	class UButton* StartTextImage;
protected:
	virtual void NativeConstruct();
public:
	UFUNCTION()
	void OnButtonLogin();
	UFUNCTION()
	void OnButtonRegistration();

	class AHttpActor* HttpActor;
	void SetHttpActor(AHttpActor* MyHttpActor);
};
