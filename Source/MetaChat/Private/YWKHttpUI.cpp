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

    // 수동으로 위젯 바인딩 (Try to bind widgets manually if auto-binding is failing)
    if (!LogCheckBox)
    {
        LogCheckBox = Cast<UTextBlock>(GetWidgetFromName(TEXT("LogCheckBox")));
        if (LogCheckBox)
        {
            UE_LOG(LogTemp, Log, TEXT("LogCheckBox is successfully bound manually in NativeConstruct."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("LogCheckBox is still null after manual binding in NativeConstruct!"));
        }
    }

    // SendButton 클릭 이벤트 바인딩
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
    // 만약 LogCheckBox가 null이면 다시 수동으로 바인딩을 시도
    if (!LogCheckBox)
    {
        LogCheckBox = Cast<UTextBlock>(GetWidgetFromName(TEXT("LogCheckBox")));
        if (!LogCheckBox)
        {
            UE_LOG(LogTemp, Error, TEXT("LogCheckBox is null, cannot update text"));
            return;
        }
    }

    // 타이머 정지 (깜빡임 멈추기)
    GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);

    // 텍스트 업데이트
    LogCheckBox->SetText(FText::FromString(NewLog));
    UE_LOG(LogTemp, Log, TEXT("LogCheckBox updated with text: %s"), *NewLog);
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
    if (SummaryTextBlock)
    {
        SummaryTextBlock->SetText(FText::FromString(SummaryText));
        if (ScrollBox)
        {
            ScrollBox->ScrollToEnd(); // 텍스트가 길어지면 자동으로 스크롤
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SummaryTextBlock not found"));
    }
}

void UYWKHttpUI::UpdateWidgetTexture()
{

}

