// Fill out your copyright notice in the Description page of Project Settings.


#include "LSJ/LoginScreenWidget.h"
#include "LSJ/LoginWidget.h"
#include "LSJ/RegisterWidget.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "LSJ/HttpActor.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "MetaChat/MetaChat.h"
#include "LSJ/JsonParseLib.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "LSJ/MetaChatGameInstance.h"

void ULoginScreenWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry,InDeltaTime);
    currentTime+=InDeltaTime;
}

void ULoginScreenWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 버튼 클릭 이벤트 바인딩
    if (  WBP_Login->ButtonLogin )
    {
        WBP_Login->ButtonLogin->OnClicked.AddDynamic(this , &ULoginScreenWidget::OnButtonLogin);
    }
    // 버튼 클릭 이벤트 바인딩
    if (  WBP_Login->ButtonRegistration )
    {
        WBP_Login->ButtonRegistration->OnClicked.AddDynamic(this , &ULoginScreenWidget::OnButtonRegistration);
    }
    WBP_Register->ButtonRegistration->OnClicked.AddDynamic(this , &ULoginScreenWidget::OnButtonRegisterSend);
    WBP_Register->ButtonRegisterClose->OnClicked.AddDynamic(this , &ULoginScreenWidget::OnButtonRegisterClose);
    ButtonToLoginScreen->OnClicked.AddDynamic(this , &ULoginScreenWidget::OnButtonToLoginScreen);
  
    WBP_Login->SetVisibility(ESlateVisibility::Hidden);
    WBP_Register->SetVisibility(ESlateVisibility::Hidden);

}

void ULoginScreenWidget::OnButtonLogin()
{
   if(nullptr == HttpActor)
        return;
        FString sEditID = WBP_Login->EditID->GetText().ToString();
    FString sEditPW = WBP_Login->EditPW->GetText().ToString();
    sEditID = sEditID.TrimStartAndEnd();
    sEditPW = sEditPW.TrimStartAndEnd();
    if (sEditID.IsEmpty() || sEditPW.IsEmpty())
    {
          ImageFailInput->SetVisibility(ESlateVisibility::Visible);
	   FTimerHandle handle;
	   GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
		   ImageFailInput->SetVisibility(ESlateVisibility::Hidden);
		   }, 3.f, false);
        return;
    }
    //UGameplayStatics::OpenLevel(GetWorld(),*HttpActor->CustomCharacterMap);

    TMap<FString, FString> LoginData;
	LoginData.Add("id",   WBP_Login->EditID->GetText().ToString());
	LoginData.Add("pass", WBP_Login->EditPW->GetText().ToString());
	FString json = UJsonParseLib::MakeJson(LoginData);

    HttpActor->RsqPostLogin(LOGIN,json);
}
void ULoginScreenWidget::OnButtonLoginResponse(FString result,int code)
{ //성공
   if (code == 200)
   {
        FString UserToken;
        FString Message;
        UJsonParseLib::JsonParseUserInfo(result,UserToken);
        if (UserToken.IsEmpty())
        {
			ImageFailLogin->SetVisibility(ESlateVisibility::Visible);
			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
				ImageFailLogin->SetVisibility(ESlateVisibility::Hidden);
			 }, 3.f, false);
        }
        else
        {
            auto* gi = Cast<UMetaChatGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
            gi->UserID =  WBP_Login->EditID->GetText().ToString();
            gi->UserToken = UserToken;
            gi->currentTime = currentTime;
            UGameplayStatics::OpenLevel(GetWorld(),*HttpActor->CustomCharacterMap);

        }
            
   }
   else if (code == 500)
   {

        
   }
}
void ULoginScreenWidget::OnButtonRegistration()
{
   if(nullptr == HttpActor)
        return;
    WBP_Register->SetVisibility(ESlateVisibility::Visible);
}

void ULoginScreenWidget::OnButtonToLoginScreen()
{
     WBP_Login->SetVisibility(ESlateVisibility::Visible);
     StartTextImage->SetVisibility(ESlateVisibility::Hidden);
}

void ULoginScreenWidget::OnButtonRegisterClose()
{
    WBP_Register->SetVisibility(ESlateVisibility::Hidden);
}

void ULoginScreenWidget::OnButtonRegisterSend()
{
    if(nullptr == HttpActor)
       return;
    FString sEditID = WBP_Register->EditID->GetText().ToString();
    FString sEditPW = WBP_Register->EditPW->GetText().ToString();
    sEditID = sEditID.TrimStartAndEnd();
    sEditPW = sEditPW.TrimStartAndEnd();
    if (sEditID.IsEmpty() || sEditPW.IsEmpty())
    {
          ImageFailInput->SetVisibility(ESlateVisibility::Visible);
	   FTimerHandle handle;
	   GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
		   ImageFailInput->SetVisibility(ESlateVisibility::Hidden);
		   }, 3.f, false);
        return;
    }
	TMap<FString, FString> RegisterData;
	RegisterData.Add("userId",   WBP_Register->EditID->GetText().ToString());
	RegisterData.Add("userPass", WBP_Register->EditPW->GetText().ToString());
	FString json = UJsonParseLib::MakeJson(RegisterData);

    HttpActor->RsqPostCreateID(REGISTERURL,json);
}
void ULoginScreenWidget::OnButtonRegisterResponse(FString result,int code)
{ //성공
   if (code == 200)
   {

       WBP_Register->SetVisibility(ESlateVisibility::Hidden);
	   ImageSuccessRegister->SetVisibility(ESlateVisibility::Visible);
	   FTimerHandle handle;
	   GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
		   ImageSuccessRegister->SetVisibility(ESlateVisibility::Hidden);

		   }, 3.f, false);
       
   }
   else if (code == 500)
   {
   		ImageFailRegister->SetVisibility(ESlateVisibility::Visible);
			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
				ImageFailRegister->SetVisibility(ESlateVisibility::Hidden);
			 }, 3.f, false);
   }
}
void ULoginScreenWidget::SetHttpActor(AHttpActor* MyHttpActor)
{
    HttpActor = MyHttpActor;
}
