
// Fill out your copyright notice in the Description page of Project Settings.


#include "LSJ/LSJMainWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "LSJ/ScreenActor.h"
void ULSJMainWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}
void ULSJMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry,InDeltaTime);
	if (bStreaming && nullptr != ScreenActor)
	{
		ScreenActor->UpdateTexture();
	}
}
void ULSJMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ButtonLookSharingScreen->OnClicked.AddDynamic(this,&ULSJMainWidget::OnButtonLookSharingScreen);
	ButtonWindowScreen->OnClicked.AddDynamic(this, &ULSJMainWidget::OnButtonWindowScreen);
	ImageSharingScreen->SetVisibility(ESlateVisibility::Hidden);
}

void ULSJMainWidget::OnButtonWindowScreen()
{
	bStreaming = !bStreaming;

	if (bStreaming)
	{
		ScreenActor->WindowScreenPlaneMesh->SetVisibility(true);
	}
	else
	{
		ScreenActor->WindowScreenPlaneMesh->SetVisibility(false);
	}
}

void ULSJMainWidget::OnButtonLookSharingScreen()
{
	bLookStreaming = !bLookStreaming;
	if (bLookStreaming)
	{
		ImageSharingScreen->SetVisibility(ESlateVisibility::Visible);
		//블루프린트 subs
		ScreenActor->BeginLookSharingScreen();
	}
	else
	{
		ImageSharingScreen->SetVisibility(ESlateVisibility::Hidden);
		//블루프린트 subs
		ScreenActor->StopLookSharingScreen();
	}
}

void ULSJMainWidget::SetScreenActor(AScreenActor* Actor)
{
	ScreenActor = Actor;
}