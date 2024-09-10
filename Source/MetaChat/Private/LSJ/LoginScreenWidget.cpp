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
     FString name = "Login";
    GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Subsystem Name : % s"), *name));
	UE_LOG(LogTemp, Warning, TEXT("OnButtonLogin"));
    FString levelName = L"Prototype";
    UGameplayStatics::OpenLevel(GetWorld(),*levelName);
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

	TMap<FString, FString> RegisterData;
	RegisterData.Add("userId",   WBP_Register->EditID->GetText().ToString());
	RegisterData.Add("userPass", WBP_Register->EditPW->GetText().ToString());
	FString json = UJsonParseLib::MakeJson(RegisterData);

    HttpActor->RsqPostCreateID(REGISTERURL,json);
    WBP_Register->EditID->GetText().ToString();
    WBP_Register->EditPW->GetText().ToString();
}

void ULoginScreenWidget::SetHttpActor(AHttpActor* MyHttpActor)
{
    HttpActor = MyHttpActor;
}
