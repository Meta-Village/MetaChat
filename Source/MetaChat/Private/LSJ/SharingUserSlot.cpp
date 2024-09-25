// Fill out your copyright notice in the Description page of Project Settings.


#include "LSJ/SharingUserSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
void USharingUserSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ViewButton->OnClicked.AddDynamic(this,&USharingUserSlot::ViewButtonOnClick);
    TextureIdle = LoadObject<UTexture2D>(nullptr, TEXT("/Game/XR_LSJ/Image/Group_5"));
	TextureClicked = LoadObject<UTexture2D>(nullptr, TEXT("/Game/XR_LSJ/Image/Group_38.Group_38"));
}

void USharingUserSlot::SetUserID(FString UserID)
{
	TextUserID->SetText(FText::FromString(UserID));
}
void USharingUserSlot::SetButtonStyle(UButton* Button, UTexture2D* NormalTexture, UTexture2D* PressedTexture, UTexture2D* HoveredTexture)
{
    if (!Button || !NormalTexture || !PressedTexture || !HoveredTexture) return;

    // 버튼 스타일 가져오기
    FButtonStyle ButtonStyle = Button->WidgetStyle;

    // Normal 상태의 이미지 설정
    FSlateBrush NormalBrush;
    NormalBrush.SetResourceObject(NormalTexture);
    ButtonStyle.SetNormal(NormalBrush);

    // Pressed 상태의 이미지 설정
    FSlateBrush PressedBrush;
    PressedBrush.SetResourceObject(PressedTexture);
    ButtonStyle.SetPressed(PressedBrush);

    // Hovered 상태의 이미지 설정
    FSlateBrush HoveredBrush;
	HoveredBrush.TintColor = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.5f)); 
    HoveredBrush.SetResourceObject(HoveredTexture);
    ButtonStyle.SetHovered(HoveredBrush);

    // 버튼에 수정된 스타일 적용
    Button->SetStyle(ButtonStyle);
}
void USharingUserSlot::ViewButtonOnClick()
{
	bClicked = !bClicked;
	//FUserIDButtonDelegate_OneParam.Execute(TextUserID->GetText().ToString());
	if (bClicked)
	{
		SetButtonStyle(ViewButton,TextureClicked,TextureClicked,TextureClicked);
	}
	else
	{
		SetButtonStyle(ViewButton,TextureIdle,TextureIdle,TextureIdle);
	}
	UserIDButtonDelegate_TwoParams.Execute(TextUserID->GetText().ToString(),bClicked);
}
