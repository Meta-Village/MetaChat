// Fill out your copyright notice in the Description page of Project Settings.


#include "LSJ/LoginScreenWidget.h"
#include "LSJ/LoginWidget.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "LSJ/HttpActor.h"

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
    WBP_Login->SetVisibility(ESlateVisibility::Hidden);
}

void ULoginScreenWidget::OnButtonLogin()
{
   if(nullptr == HttpActor)
        return;
     FString name = "Login";
    GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Subsystem Name : % s"), *name));
	UE_LOG(LogTemp, Warning, TEXT("OnButtonLogin"));
}

void ULoginScreenWidget::OnButtonRegistration()
{
   if(nullptr == HttpActor)
        return;
}

void ULoginScreenWidget::SetHttpActor(AHttpActor* MyHttpActor)
{
    HttpActor = MyHttpActor;
}
