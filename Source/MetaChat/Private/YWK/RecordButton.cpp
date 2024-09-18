// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/RecordButton.h"
#include "Components/TextBlock.h" 
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "HSB/CustomCharacter.h"
#include "Kismet/GameplayStatics.h"



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
	ACustomCharacter* PlayerCharacter = Cast<ACustomCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	if (PlayerCharacter)
	{
		// 현재 플레이어가 있는 존
		FString ZoneName = PlayerCharacter->GetCurrentZoneName();

		UE_LOG(LogTemp, Log, TEXT("Current Zone: %s"), *ZoneName);

		// 1,2번 방이면 보이게
		if (ZoneName == "ROOM1" || ZoneName == "ROOM2")
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

