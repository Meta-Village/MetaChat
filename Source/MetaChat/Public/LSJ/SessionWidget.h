// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionWidget.generated.h"

/**
 * 
 */
UCLASS()
class METACHAT_API USessionWidget : public UUserWidget
{
	GENERATED_BODY()
	bool bCreateSession;
protected:
	virtual void NativeConstruct();
public:
	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonClose;
	UFUNCTION()
	void ClosePopup();
	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonCreateSession;
	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonJoinSession;
	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonComfirm;
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* PanelButton;
	UPROPERTY(meta=(BindWidget))
	class UCanvasPanel* PanelSession;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextSessionName;
	UPROPERTY(meta=(BindWidget))
	class UImage* ImageFailInput;
	UPROPERTY(meta=(BindWidget))
	class UEditableText* RoomNum;
	UPROPERTY(meta=(BindWidget))
	class UEditableText* RoomPwd;
	UPROPERTY(meta=(BindWidget))
	class UImage* ImageDisConnect;
	
	void OnClickPlaySound();
	UFUNCTION()
	void OnButtonConfirm();
	UFUNCTION()
	void OpenCreateSessionScreen();
	UFUNCTION()
	void OpenJoinSessionScreen();
	UFUNCTION(BlueprintCallable)
	void SendFindSessionInfo(FString SendWorldID);
	UFUNCTION(BlueprintCallable)
	void SendCreatingWorldInfo();
	void RecvCreatingWorldInfo(FString result, int32 resultCode);
	void RecvFindSessionInfo(FString result, int32 resultCode);
	void SetHttpActor(class AHttpActor* actor);
	//세션 생성하는 블루프린트 함수
	UFUNCTION(BlueprintImplementableEvent, Category = "MyCategory")
    void CreateSession(int WorldID);
	//세션 참가하는 블루프린트 함수
	UFUNCTION(BlueprintImplementableEvent, Category = "MyCategory")
    void JoinSession(int WorldID);

	UPROPERTY(BlueprintReadWrite)
	FString WorldName;
	UPROPERTY(BlueprintReadWrite)
	FString WorldPassword;

	class AHttpActor* HttpActor;
	//UPROPERTY(meta=(BindWidget))
	//class UImage* ImageFailLogin;
	//	UPROPERTY(meta=(BindWidget))
	//class UImage* ImageFailRegister;
	//		UPROPERTY(meta=(BindWidget))
	//class UImage* ImageSuccessRegister;
	//			UPROPERTY(meta=(BindWidget))
	//class UImage* ImageFailInput;
};
