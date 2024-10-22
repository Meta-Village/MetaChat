﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "LSJ/SessionWidget.h"
#include "LSJ/HttpActor.h"
#include "LSJ/JsonParseLib.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"
#include "Components/Image.h"
#include "LSJ/MetaChatGameInstance.h"
#include "Kismet/GameplayStatics.h"
void USessionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ButtonCreateSession->OnClicked.AddDynamic(this,&USessionWidget::OpenCreateSessionScreen);
	ButtonJoinSession->OnClicked.AddDynamic(this,&USessionWidget::OpenJoinSessionScreen);
	ButtonComfirm->OnClicked.AddDynamic(this,&USessionWidget::OnButtonConfirm);
	ButtonClose->OnClicked.AddDynamic(this,&USessionWidget::ClosePopup);
}

void USessionWidget::ClosePopup()
{
	OnClickPlaySound();
	PanelSession->SetVisibility(ESlateVisibility::Hidden);
}

void USessionWidget::OnClickPlaySound()
{
	auto* gi = Cast<UMetaChatGameInstance>(GetWorld()->GetGameInstance());
	if(gi)
		UGameplayStatics::PlaySound2D(GetWorld(),gi->ButtonSound);
}

void USessionWidget::OnButtonConfirm()
{
	OnClickPlaySound();
	WorldName = RoomNum->GetText().ToString();
	WorldPassword = RoomPwd->GetText().ToString();
	WorldName = WorldName.TrimStartAndEnd();
	WorldPassword = WorldPassword.TrimStartAndEnd();
    if (WorldName.IsEmpty() || WorldPassword.IsEmpty())
    {
          ImageFailInput->SetVisibility(ESlateVisibility::Visible);
	   FTimerHandle handle;
	   GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
		   ImageFailInput->SetVisibility(ESlateVisibility::Hidden);
		   }, 3.f, false);
        return;
    }
	if (bCreateSession)
	{
		SendCreatingWorldInfo();
	}
	else
	{
		SendFindSessionInfo(WorldName);
	}
}

void USessionWidget::OpenCreateSessionScreen()
{
	OnClickPlaySound();
	bCreateSession = true;
	PanelSession->SetVisibility(ESlateVisibility::Visible);
	TextSessionName->SetText(FText::FromString(TEXT("회의실명")));
}

void USessionWidget::OpenJoinSessionScreen()
{
	OnClickPlaySound();
	bCreateSession = false;
	PanelSession->SetVisibility(ESlateVisibility::Visible);
	TextSessionName->SetText(FText::FromString(TEXT("코드")));
}

void USessionWidget::SendFindSessionInfo(FString SendWorldID)
{
	//FString createWorldURL = "http://125.132.216.190:8126/api/v1/worlds/";
	//FString fullURL = FString::Printf(TEXT("%s%s"), *createWorldURL, *SendWorldID);
	FString fullURL = "http://125.132.216.190:8126/api/v1/worlds/";
	fullURL = FString::Printf(TEXT("%s%s/validate-password"), *fullURL,*WorldName);

	TMap<FString, FString> WorldData;
	WorldData.Add("password", WorldPassword);
	FString json = UJsonParseLib::MakeJson(WorldData);

	HttpActor->RsqPostJoinWorld(fullURL, json);
}


void USessionWidget::SendCreatingWorldInfo()
{
	//서버에 월드제목과 월드비밀번호 보내고 결과 받기
	FString createWorldURL = "http://125.132.216.190:8126/api/v1/worlds";

	TMap<FString, FString> WorldData;
	WorldData.Add("worldName", WorldName);
	WorldData.Add("worldPassword", WorldPassword);
	FString json = UJsonParseLib::MakeJson(WorldData);

	HttpActor->RsqPostCreateWorld(createWorldURL, json);

	//FString createWorldURL2 = "http://125.132.216.190:8126/api/v1/worlds";
	//HttpActor->RsqGetTest(createWorldURL);
}

void USessionWidget::RecvCreatingWorldInfo(FString result, int32 resultCode)
{
	if (resultCode == 200 || resultCode == 201)
	{
		int32 RecvWorldID;
		UJsonParseLib::JsonParsePassword(result, RecvWorldID, WorldName);
		
		auto* gi = Cast<UMetaChatGameInstance>(GetWorld()->GetGameInstance());
		gi->WorldID =RecvWorldID;
		gi->WorldPW = WorldPassword;
		gi->WorldName = WorldName;
		CreateSession(RecvWorldID);
		RemoveFromParent();
	}
	
}

void USessionWidget::RecvFindSessionInfo(FString result, int32 resultCode)
{
	if (resultCode == 200 || resultCode == 201)
	{
		//int32 RecvWorldID;
		//UJsonParseLib::JsonParsePassword(result, RecvWorldID, WorldName);
		auto* gi = Cast<UMetaChatGameInstance>(GetWorld()->GetGameInstance());
		gi->WorldID =FCString::Atoi(*WorldName);
		gi->WorldPW = WorldPassword;
		JoinSession(FCString::Atoi(*WorldName));
	}
	else if (resultCode == 404)
	{
		ImageDisConnect->SetVisibility(ESlateVisibility::Visible);
		 FTimerHandle handle;
	   GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
		   ImageDisConnect->SetVisibility(ESlateVisibility::Hidden);
		   PanelSession->SetVisibility(ESlateVisibility::Hidden);
		   }, 3.f, false);
	}
}

void USessionWidget::SetHttpActor(AHttpActor* actor)
{
	HttpActor = actor;
}
