// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/RecordButton.h"
#include "Components/TextBlock.h" 
#include "Components/WidgetSwitcher.h"

void URecordButton::NativeConstruct()
{
	Super::NativeConstruct();

	// 플레이와 스탑버튼 바인딩
}

//void URecordButton::OnPlayButtonClicked()
//{
//	//// 텍스트 블럭을 visible로 설정
//	if (TextBlock_96)
//	{
//		TextBlock_96->SetVisibility(ESlateVisibility::Visible);
//	}
//	// 위젯 스위처를 1로 변경
//	if (WidgetSwitcher_50)
//	{
//		WidgetSwitcher_50->SetActiveWidgetIndex(1);
//	}
//	//// 녹음중
//	StartRecording();
//}
//
//void URecordButton::OnStopButtonClicked()
//{
//	// 텍스트 블럭을 Hidden로 설정
//	if (TextBlock_96)
//	{
//		TextBlock_96->SetVisibility(ESlateVisibility::Hidden);
//	}
//	// 위젯 스위처를 0으로 변경
//	if (WidgetSwitcher_50)
//	{
//		WidgetSwitcher_50->SetActiveWidgetIndex(0);
//	}
//	// 녹음 끄기
//	StopRecording();
//}
//
//void URecordButton::StartRecording()
//{
//
//}
//
//void URecordButton::StopRecording()
//{
//
//}
