// Fill out your copyright notice in the Description page of Project Settings.


#include "LSJ/SharingUserSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
void USharingUserSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ViewButton->OnClicked.AddDynamic(this,&USharingUserSlot::ViewButtonOnClick);
}

void USharingUserSlot::SetUserID(FString UserID)
{
	TextUserID->SetText(FText::FromString(UserID));
}

void USharingUserSlot::ViewButtonOnClick()
{
	FUserIDButtonDelegate_OneParam.Execute(TextUserID->GetText().ToString());
}
