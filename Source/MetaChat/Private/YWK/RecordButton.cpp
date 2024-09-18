// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/RecordButton.h"
#include "Components/TextBlock.h" 
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "HSB/CustomCharacter.h"
#include "Kismet/GameplayStatics.h"



// 플레이어의 이전 존을 저장할 변수
FString PreviousZoneName = TEXT("");

void URecordButton::NativeConstruct()
{
	Super::NativeConstruct();

	// 맨 처음 위젯스위쳐 숨기기
	VisibleSwitcher(false);
}

void URecordButton::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 현재 플레이어 캐릭터 가져오기
	ACustomCharacter* PlayerCharacter = Cast<ACustomCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter)
	{
		// 현재 플레이어가 있는 존
		FString CurrentZoneName = PlayerCharacter->GetCurrentZoneName();

		// 존이 변경되었을 때만 로그 출력
		if (CurrentZoneName != PreviousZoneName)
		{
			UE_LOG(LogTemp, Log, TEXT("Current Zone: %s"), *CurrentZoneName);
			PreviousZoneName = CurrentZoneName;  // 이전 존 업데이트
		}

		// 존이 ROOM1 또는 ROOM2이면 위젯을 보이게 설정
		if (CurrentZoneName == "ROOM1" || CurrentZoneName == "ROOM2" || CurrentZoneName == "ROOM3" || CurrentZoneName == "ROOM4")
		{
			VisibleSwitcher(true);
		}
		else
		{
			VisibleSwitcher(false);
		}
	}
}

void URecordButton::VisibleSwitcher(bool bIsVisible)
{
	if (WidgetSwitcher_50)
	{
		// 보여야 할 때
		if (bIsVisible)
		{
			//보이기
			WidgetSwitcher_50->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			// 숨기기
			WidgetSwitcher_50->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

