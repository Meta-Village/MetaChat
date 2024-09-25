// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/EmojiWidget.h"
#include "Components/Image.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "ImageUtils.h"
#include "Components/BillboardComponent.h"

void UEmojiWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 평소에 안보이게 해놓기
	if (IMG_Emoji)
	{
		IMG_Emoji->SetVisibility(ESlateVisibility::Hidden);
	}
	// 빌보드 하기
	if (EmojiBillboard)
	{
		EmojiBillboard->SetWorldRotation(GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraRotation());
	}
}


void UEmojiWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 카메라 방향으로 빌보드 회전
	if (EmojiBillboard)
	{
		FRotator CameraRotation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraRotation();
		EmojiBillboard->SetWorldRotation(CameraRotation);
	}
}


void UEmojiWidget::OnReCeiveEmoji(const FString& Filename)
{
	// 서버에서 이모티콘 받아올 함수
	ServerShowEmoji(Filename);
}

void UEmojiWidget::LoadEmojiFromUrl(const FString& ImageUrl)
{
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http)
	{
		return;
	}
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UEmojiWidget::OnImageDownloaded);
	Request->SetURL(ImageUrl);
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

// 이미지 다운로드 완료 후 호출될 콜백함수
void UEmojiWidget::OnImageDownloaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		// 응답 데이터를 바이트 배열로 가져오기
		TArray<uint8> ImageData = Response->GetContent();

		// 이미지를 텍스쳐로 변환
		UTexture2D* LoadedTexture = FImageUtils::ImportBufferAsTexture2D(ImageData);

		// 텍스쳐가 유효하면 UI에 넣기
		if (LoadedTexture && IMG_Emoji)
		{
			IMG_Emoji->SetBrushFromTexture(LoadedTexture);
			IMG_Emoji->SetVisibility(ESlateVisibility::Visible);

			// 3초 뒤에 이모티콘 이미지 숨기기
			GetWorld()->GetTimerManager().ClearTimer(HideEmojiTimerHandle);
			GetWorld()->GetTimerManager().SetTimer(HideEmojiTimerHandle, this, &UEmojiWidget::HideEmoji, 3.0f, false);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("이미지 다운로드 실패"));
	}
}

// 이모티콘 숨기기 
void UEmojiWidget::HideEmoji()
{
	if (IMG_Emoji)
	{
		IMG_Emoji->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UEmojiWidget::ServerShowEmoji_Implementation(const FString& Filename)
{
	// 서버에서 파일 URL을 받아오기
	FString ApiUrl = FString::Printf(TEXT("http://125.132.216.190:8126/api/v1/files/url?filename=%s"), *Filename);

	// URL을 모든 클라이언트에 전송
	MultiShowEmoji(ApiUrl);
}

void UEmojiWidget::MultiShowEmoji_Implementation(const FString& ImageUrl)
{
	// 받은 URL을 이용해 이미지를 클라이언트에서 로드
	LoadEmojiFromUrl(ImageUrl);
}