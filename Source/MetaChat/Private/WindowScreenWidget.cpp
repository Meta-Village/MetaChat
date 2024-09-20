// Fill out your copyright notice in the Description page of Project Settings.


#include "WindowScreenWidget.h"
#include "Components/Image.h"


void UWindowScreenWidget::SetImage(UTexture2D* NewImage)
{
    if (ImageWindowScreen && NewImage)
    {
        // 이미지 브러쉬를 생성하고 텍스처로 설정
        FSlateBrush Brush;
        Brush.SetResourceObject(NewImage);
        ImageWindowScreen->SetBrush(Brush);
    }
}