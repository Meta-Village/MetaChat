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
	class URegisterWidget* WBP_Register;
	UPROPERTY(meta=(BindWidget))
	class UImage* StartTextImage;
	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonToLoginScreen;
	UPROPERTY(meta=(BindWidget))
	class UImage* ImageFailLogin;
		UPROPERTY(meta=(BindWidget))
	class UImage* ImageFailRegister;
			UPROPERTY(meta=(BindWidget))
	class UImage* ImageSuccessRegister;
				UPROPERTY(meta=(BindWidget))
	class UImage* ImageFailInput;
	
protected:
	virtual void NativeConstruct();
public:
	UFUNCTION()
	void OnButtonLogin();
	void OnButtonLoginResponse(FString result, int code);
	UFUNCTION()
	void OnButtonRegistration();
	UFUNCTION()
	void OnButtonToLoginScreen();
	UFUNCTION()
	void OnButtonRegisterClose();
	UFUNCTION()
	void OnButtonRegisterSend();

	void OnButtonRegisterResponse(FString result, int code);
	class AHttpActor* HttpActor;
	void SetHttpActor(AHttpActor* MyHttpActor);
};
