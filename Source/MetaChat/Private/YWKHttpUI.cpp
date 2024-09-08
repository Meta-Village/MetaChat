// Fill out your copyright notice in the Description page of Project Settings.


#include "YWKHttpUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "YWK/YWKHttpActor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ScrollBox.h"

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

    // 텍스트를 "회의 요약 중..." 으로 설정
    if (LogCheckBox)
    {
        LogCheckBox->SetText(FText::FromString(TEXT("회의 내용 요약중.....")));
        // 타이머 0.5초 간격으로 깜빡이게
        GetWorld()->GetTimerManager().SetTimer(BlinkTimerHandle, this, &UYWKHttpUI::ToggleBlinkingText, 0.5f, true);
    }

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

        // meetingId 설정 (예시: 사용자가 입력한 값을 가져온다거나, 특정 로직에 의해 할당됨)
        FString MeetingId = "22"; // 실제 MeetingId 값 설정

        // 파일을 서버로 전송
        YWKHttpActor->RsqPostwavfile(TEXT("http://125.132.216.190:8126/voice"), FilePath, MeetingId);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("HttpActor not found in the world"));
    }
}

void UYWKHttpUI::SetTextLog(const FString& NewLog)
{
    // 타이머 정지 (깜빡임 멈추기)
    GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);

	if (LogCheckBox)
	{
		// TextBlock에 텍스트를 설정
		LogCheckBox->SetText(FText::FromString(NewLog));
	}
}

void UYWKHttpUI::ToggleBlinkingText()
{
    if (LogCheckBox) //유효성 확인
    {
        if (bIsTextVisible) // 현재 텍스트 보이는지 확인
        {
            LogCheckBox->SetVisibility(ESlateVisibility::Hidden); //텍스트 숨김
        }
        else
        {
            LogCheckBox->SetVisibility(ESlateVisibility::Visible); // 보이기
        }
        // 토글
        bIsTextVisible = !bIsTextVisible; // 다음 호출 때는 상태 반전
    }

}

// 요약본 관련
void UYWKHttpUI::UpdateSummaryText(const FString& SummaryText)
{
    if (SummaryTextBlock) // 텍스트 블럭이 UMG에서 바인딩된 변수
    {
        SummaryTextBlock->SetText(FText::FromString(SummaryText));
        if (ScrollBox)
        {
            ScrollBox->ScrollToEnd(); // 텍스트 길어지면 자동으로 스크롤
        }
    }
}

void UYWKHttpUI::UpdateWidgetTexture()
{

}

