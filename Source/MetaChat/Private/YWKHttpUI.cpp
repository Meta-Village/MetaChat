// Fill out your copyright notice in the Description page of Project Settings.


#include "YWKHttpUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "YWK/YWKHttpActor.h"

void UYWKHttpUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (SendButton)
	{
		SendButton->OnClicked.AddDynamic(this, &UYWKHttpUI::OnSendButtonClicked);
	}
}

void UYWKHttpUI::OnSendButtonClicked()
{
	// 여기서 파일을 서버로 전송하는 로직을 호출
	UE_LOG(LogTemp, Log, TEXT("Send Button Clicked"));

	// Http 요청을 처리할 액터를 가져와서 요청 보내기
	if(AYWKHttpActor* YWKHttpActor = Cast<AYWKHttpActor>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		// 파일 경로 설정
		FString FilePath = FPaths::ProjectDir() + TEXT("savewave/test.wav");
		YWKHttpActor->RsqPostwavfile(TEXT("http://125.132.216.190:8126/api/v1/files/upload"), FilePath);
	}
}

void UYWKHttpUI::UpdateWidgetTexture()
{

}

void UYWKHttpUI::SetTextLog(const FString& NewLog)
{
	if (LogCheckBox)
	{
		// TextBlock에 텍스트를 설정
		LogCheckBox->SetText(FText::FromString(NewLog));
	}
}

