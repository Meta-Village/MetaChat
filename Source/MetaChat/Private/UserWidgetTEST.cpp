// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidgetTEST.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "MetaChatGameInstance.h"
void UUserWidgetTEST::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	//ButtonTest->OnClicked.AddDynamic(this,&UUserWidgetTEST::TestButtonCallback);
	//ButtonJoin->OnClicked.AddDynamic(this, &UUserWidgetTEST::JoinCallback);
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