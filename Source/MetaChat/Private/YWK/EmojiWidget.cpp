// Fill out your copyright notice in the Description page of Project Settings.

#include "YWK/EmojiWidget.h"
#include "Components/Image.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "ImageUtils.h"
#include "Components/BillboardComponent.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

void UEmojiWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Log, TEXT("UEmojiWidget::NativeConstruct - Constructing Widget"));

    // IMG_Emoji 초기화 상태 확인
    if (IMG_Emoji)
    {
        IMG_Emoji->SetVisibility(ESlateVisibility::Hidden);
        UE_LOG(LogTemp, Log, TEXT("IMG_Emoji is successfully bound and hidden."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("IMG_Emoji is null in NativeConstruct."));
    }

    // 빌보드 설정 전에 null 체크
    if (!EmojiBillboard)
    {
        UE_LOG(LogTemp, Error, TEXT("EmojiBillboard is null in NativeConstruct."));
    }
    else
    {
        // PlayerController 및 PlayerCameraManager 유효성 체크
        APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
        if (PlayerController && PlayerController->PlayerCameraManager)
        {
            EmojiBillboard->SetWorldRotation(PlayerController->PlayerCameraManager->GetCameraRotation());
            UE_LOG(LogTemp, Log, TEXT("EmojiBillboard is set to camera rotation"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("PlayerController or PlayerCameraManager is null."));
        }
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

// 서버로 이모티콘 요청을 보내는 함수
void UEmojiWidget::SendEmojiRequestToServer(const FString& ChatMessage)
{
    // HTTP 모듈 생성
    FHttpModule* Http = &FHttpModule::Get();
    if (!Http)
    {
        UE_LOG(LogTemp, Error, TEXT("FHttpModule is not valid"));
        return;
    }

    // Json 형식으로 서버에 보낼 데이터 생성
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    JsonObject->SetStringField(TEXT("minutes"), ChatMessage);

    FString OutputString;
    TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    // HTTP 요청 설정
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &UEmojiWidget::OnEmojiResponseReceived);
    Request->SetURL(TEXT("https://eagle-prepared-octopus.ngrok-free.app/chatbot/emote"));  // FastAPI 라우트 URL
    Request->SetVerb("POST");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetContentAsString(OutputString);
    Request->ProcessRequest();
}

void UEmojiWidget::OnEmojiResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("Emoji response received from server: %s"), *Response->GetContentAsString());

        // 서버에서 반환된 이미지 파일 경로 가져오기
        FString ImageUrl = Response->GetContentAsString(); // 서버 응답에 맞춰 파싱
        LoadEmojiFromUrl(ImageUrl);  // 이미지 URL을 통해 GET 요청으로 이미지 다운로드
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to receive response from server"));
    }
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

    // HTTP GET 요청 생성 및 처리
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

        // 이미지를 텍스처로 변환
        UTexture2D* LoadedTexture = FImageUtils::ImportBufferAsTexture2D(ImageData);

        // 텍스처가 유효하면 UI에 표시
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
        UE_LOG(LogTemp, Error, TEXT("Image download failed"));
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
    UE_LOG(LogTemp, Log, TEXT("UEmojiWidget::ServerShowEmoji - Requesting Emoji with filename: %s"), *Filename);

    // 서버로 POST 요청 전송
    SendEmojiRequestToServer(Filename);
}

void UEmojiWidget::SetEmojiImageFromData(const TArray<uint8>& ImageData)
{
    if (!IMG_Emoji)
    {
        UE_LOG(LogTemp, Error, TEXT("IMG_Emoji is null when trying to assign texture. Delaying texture assignment."));
        // 0.1초 후에 다시 시도하도록 타이머 설정
        GetWorld()->GetTimerManager().SetTimerForNextTick([this, ImageData]()
        {
            SetEmojiImageFromData(ImageData);  // 다시 시도
        });
        return;
    }

    // ImageWrapper 모듈 로드
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

    // PNG 포맷에 대한 ImageWrapper 생성
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
    if (!ImageWrapper.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create ImageWrapper"));
        return;
    }

    // 이미지 압축 데이터 설정
    if (!ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to set compressed image data"));
        return;
    }

    // 압축 해제된 이미지 데이터를 저장할 배열
    TArray<uint8> UncompressedData;
    if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedData))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to uncompress image data"));
        return;
    }

    // 텍스처 생성
    UTexture2D* NewTexture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);
    if (!NewTexture)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create texture"));
        return;
    }

    // 텍스처 데이터를 잠그고, 압축 해제된 데이터를 복사
    void* TextureData = NewTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
    FMemory::Memcpy(TextureData, UncompressedData.GetData(), UncompressedData.Num());
    NewTexture->GetPlatformData()->Mips[0].BulkData.Unlock();

    // 텍스처 리소스 업데이트
    NewTexture->UpdateResource();

    // UI에 텍스처를 적용하고 표시
    IMG_Emoji->SetBrushFromTexture(NewTexture);
    IMG_Emoji->SetVisibility(ESlateVisibility::Visible);

    UE_LOG(LogTemp, Log, TEXT("Emoji image successfully displayed"));

    // 3초 후 이모티콘을 숨김
    GetWorld()->GetTimerManager().ClearTimer(HideEmojiTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(HideEmojiTimerHandle, this, &UEmojiWidget::HideEmoji, 3.0f, false);
}