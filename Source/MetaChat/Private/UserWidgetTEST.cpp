// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidgetTEST.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "MetaChatGameInstance.h"
#include "JsonParseLib.h"
#include "Components/TextBlock.h"
#include "HttpActor.h"
void UUserWidgetTEST::NativeConstruct()
{
	Super::NativeConstruct();
	//버튼 연결
	//ButtonTest->OnClicked.AddDynamic(this,&UUserWidgetTEST::TestButtonCallback);
	//ButtonJoin->OnClicked.AddDynamic(this, &UUserWidgetTEST::JoinCallback);
	HttpGetButton->OnClicked.AddDynamic(this, &UUserWidgetTEST::OnMyClickSendGet);
	HttpPostButton->OnClicked.AddDynamic(this, &UUserWidgetTEST::OnMyClickSendPost);
}


void UUserWidgetTEST::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}
void UUserWidgetTEST::SetTextLog(FString log)
{
	TextLog->SetText(FText::FromString(log));
}
void UUserWidgetTEST::TestButtonCallback()
{
	UMetaChatGameInstance* gi = Cast<UMetaChatGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	gi->CreateGameSession();
}
void UUserWidgetTEST::JoinCallback()
{
	UMetaChatGameInstance* gi = Cast<UMetaChatGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	gi->JoinGameSession();
}

void UUserWidgetTEST::OnMyClickSendGet()
{
	//httpActor에게 공공데이터값을 받아오라고 요청 하고 싶다.
	//FString fullURL = FString::Printf(TEXT("%s?serviceKey=%s&pageNo=%s&numOfRows=%s"), *url, *key, *EditPageNo->GetText().ToString(), *EditNumOfRows->GetText().ToString());
	FString fullURL = "http://master-of-prediction.shop:8123/test";
	HttpActor->RsqGetTest(fullURL);
}

void UUserWidgetTEST::OnMyClickSendPost()
{
	TMap<FString, FString> studentData;
	studentData.Add("Name", "Kei");
	studentData.Add("Age", "23");
	studentData.Add("Height", "185");
	FString json = UJsonParseLib::MakeJson(studentData);
	FString fullURL = "http://master-of-prediction.shop:8123/test";
	HttpActor->RsqPostTest(fullURL, json);
}

void UUserWidgetTEST::SetHttpActor(AHttpActor* actor)
{
	HttpActor = actor;
}