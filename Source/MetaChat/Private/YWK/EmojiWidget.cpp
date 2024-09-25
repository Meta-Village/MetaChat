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

	UE_LOG(LogTemp, Log, TEXT("UEmojiWidget::NativeConstruct - Constructing Widget"));

	// 평소에 안보이게 해놓기
	if (IMG_Emoji)
	{
		IMG_Emoji->SetVisibility(ESlateVisibility::Hidden);
		UE_LOG(LogTemp, Log, TEXT("IMG_Emoji is hidden"));
	}
	// 빌보드 하기
	if (EmojiBillboard)
	{
		EmojiBillboard->SetWorldRotation(GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraRotation());
		UE_LOG(LogTemp, Log, TEXT("EmojiBillboard is set to camera rotation"));
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
		UE_LOG(LogTemp, Log, TEXT("UEmojiWidget::NativeTick - EmojiBillboard rotated to camera direction"));
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
		UE_LOG(LogTemp, Error, TEXT("FHttpModule is not valid"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("UEmojiWidget::LoadEmojiFromUrl - Downloading image from URL: %s"), *ImageUrl);

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

		UE_LOG(LogTemp, Log, TEXT("Image download successful"));

		// 응답 데이터를 바이트 배열로 가져오기
		TArray<uint8> ImageData = Response->GetContent();

		// 이미지를 텍스쳐로 변환
		UTexture2D* LoadedTexture = FImageUtils::ImportBufferAsTexture2D(ImageData);

		// 텍스쳐가 유효하면 UI에 넣기
		if (LoadedTexture && IMG_Emoji)
		{
			IMG_Emoji->SetBrushFromTexture(LoadedTexture);
			IMG_Emoji->SetVisibility(ESlateVisibility::Visible);

			UE_LOG(LogTemp, Log, TEXT("Image successfully set to IMG_Emoji"));

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
	UE_LOG(LogTemp, Log, TEXT("UEmojiWidget::ServerShowEmoji - Requesting Emoji with filename : %s"), *Filename);

	// 서버에서 파일 URL을 받아오기
	FString ApiUrl = FString::Printf(TEXT("http://125.132.216.190:8126/api/v1/files/url?filename=%s"), *Filename);
	UE_LOG(LogTemp, Log, TEXT("API URL generated : %s"), *ApiUrl);

	// URL을 모든 클라이언트에 전송
	MultiShowEmoji(ApiUrl);
}

void UEmojiWidget::MultiShowEmoji_Implementation(const FString& ImageUrl)
{
	UE_LOG(LogTemp, Log, TEXT("UEmojiWidget::MultiShowEmoji - Loading Emoji from URL : %s"), *ImageUrl);
	// 받은 URL을 이용해 이미지를 클라이언트에서 로드
	LoadEmojiFromUrl(ImageUrl);
}