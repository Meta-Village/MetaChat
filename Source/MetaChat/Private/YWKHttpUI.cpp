// Fill out your copyright notice in the Description page of Project Settings.


#include "YWKHttpUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "YWK/YWKHttpActor.h"
#include "Kismet/GameplayStatics.h"

void UYWKHttpUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (SendButton)
	{
		SendButton->OnClicked.AddDynamic(this, &UYWKHttpUI::OnSendButtonClicked);
	}
}

// 파일 전송 버튼 클릭
void UYWKHttpUI::OnSendButtonClicked()
{
    UE_LOG(LogTemp, Log, TEXT("Send Button Clicked"));

    // 월드에서 AYWKHttpActor 액터 찾기
    AYWKHttpActor* YWKHttpActor = Cast<AYWKHttpActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AYWKHttpActor::StaticClass()));

    if (YWKHttpActor)
    {
        // 파일 경로 설정
        FString FilePath = FPaths::ProjectDir() + TEXT("savewave/test.wav");

        // 파일 존재여부 확인
        if (!FPaths::FileExists(FilePath))
        {
            UE_LOG(LogTemp, Error, TEXT("File Not Found: %s"), *FilePath);
            return;
        }

        // 파일을 서버로 전송
        YWKHttpActor->RsqPostwavfile(TEXT("http://125.132.216.190:8126/api/v1/files/upload"), FilePath);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("HttpActor not found in the world"));
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

