// Fill out your copyright notice in the Description page of Project Settings.


#include "LSJ/PixelStreamingActor.h"
//#include "IPixelStreamingModule.h"
//#include "Modules/ModuleManager.h"
#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreaming/Private/PixelStreamingModule.h"
//#include "PixelStreamingModule.h" 

//#include "Modules/ModuleManager.h"

#include "Modules/ModuleManager.h"
#include "Widgets/SViewport.h"
#include "Slate/SceneViewport.h"

//window viewer
#include "Components/StaticMeshComponent.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture2DDynamic.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#include <vector> 
#include "Styling/SlateBrush.h"
#include "Components/Image.h"
#include "Rendering/Texture2DResource.h"
APixelStreamingActor::APixelStreamingActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // Plane Mesh 초기화
    PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh"));
    //PlaneMesh->SetupAttachment(RootComponent);
    SetRootComponent(PlaneMesh);
    // 기본 Plane Mesh 설정
    static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMeshAsset(TEXT("/Engine/BasicShapes/Plane.Plane"));
    if ( PlaneMeshAsset.Succeeded() )
    {
        PlaneMesh->SetStaticMesh(PlaneMeshAsset.Object);
    }

    // 기본 머티리얼 설정
    static ConstructorHelpers::FObjectFinder<UMaterial> DefaultMaterial(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/XR_LSJ/WindowViewer/MMMMM.MMMMM'"));
    if ( DefaultMaterial.Succeeded() )
    {
        PlaneMesh->SetMaterial(0 , DefaultMaterial.Object);
    }
}

void APixelStreamingActor::BeginPlay()
{
    Super::BeginPlay();

    StartPixelStreaming();
    DynamicMaterial = UMaterialInstanceDynamic::Create(PlaneMesh->GetMaterial(0) , this);
    // PlaneMesh에 머티리얼 적용
    PlaneMesh->SetMaterial(0 , DynamicMaterial);
}

void APixelStreamingActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    if ( AsyncTaskHandle.IsValid() && !AsyncTaskHandle.IsReady() )
    {
        AsyncTaskHandle.Wait();  // 필요시 Task가 종료되도록 대기
    }
    StopPixelStreaming();

}

void APixelStreamingActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    UpdateWidgetTexture();
}

void APixelStreamingActor::UpdateWidgetTexture()
{
    // 일정 시간 간격으로 화면 캡처 수행
    bIsTaskCancelled = false;
    static float TimeAccumulator = 0.0f;
    const float CaptureInterval = 0.1f; // 1초에 한 번 캡처
    TimeAccumulator += GetWorld()->GetDeltaSeconds();

    if ( TimeAccumulator >= CaptureInterval )
    {
        TimeAccumulator = 0.0f;

        // 비동기 작업을 관리하는 TFuture를 사용
        AsyncTaskHandle = Async(EAsyncExecution::ThreadPool , [this]()
        {
            if ( bIsTaskCancelled || IsEngineExitRequested() || !IsValid(this) )
            {
                return;  // 작업 취소 시 즉시 반환
            }
            UTexture2D* CapturedTexture = nullptr;
            CapturedTexture = CaptureScreenToTexture();

            // 메인 스레드에서 UI 업데이트
            AsyncTask(ENamedThreads::GameThread , [this , CapturedTexture]()
            {
                if ( CapturedTexture && !bIsTaskCancelled && IsValid(this))
                {
    
       
      
                    
                      

                       
                        // BaseTexture 파라미터에 텍스처 설정
                        DynamicMaterial->SetTextureParameterValue(TEXT("Base") , CapturedTexture);
              
                    

                }
            });
        });
    }
}

UTexture2D* APixelStreamingActor::CaptureScreenToTexture()
{
    int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    // HDC 및 메모리 장치 컨텍스트 생성
    HDC hScreenDC = GetDC(NULL);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC , ScreenWidth , ScreenHeight);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC , hBitmap);

    // 화면을 메모리 DC로 복사
    BitBlt(hMemoryDC , 0 , 0 , ScreenWidth , ScreenHeight , hScreenDC , 0 , 0 , SRCCOPY);

    // 비트맵 데이터를 저장할 배열 생성
    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = ScreenWidth;
    bi.biHeight = -ScreenHeight;  // 비트맵이 상하 반전되지 않도록 음수 설정
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    TArray<uint8> BitmapData;
    BitmapData.SetNumUninitialized(ScreenWidth * ScreenHeight * 4);
    GetDIBits(hMemoryDC , hBitmap , 0 , ScreenHeight , BitmapData.GetData() , (BITMAPINFO*)&bi , DIB_RGB_COLORS);

    // 텍스처 생성
    UTexture2D* Texture = UTexture2D::CreateTransient(ScreenWidth , ScreenHeight , PF_B8G8R8A8);
    if ( Texture )
    {
        // 텍스처에 데이터를 업데이트하기 위한 영역 정의
        FUpdateTextureRegion2D UpdateRegion(0 , 0 , 0 , 0 , ScreenWidth , ScreenHeight);

        // 텍스처 업데이트
        Texture->UpdateTextureRegions(0 , 1 , &UpdateRegion , ScreenWidth * 4 , 4 , BitmapData.GetData());
    }

    // 리소스 해제
    SelectObject(hMemoryDC , hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL , hScreenDC);

    return Texture;
}
void APixelStreamingActor::StartServers()
{
    // 배치 파일의 전체 경로
    FString BatFilePath = FPaths::Combine(FPaths::EnginePluginsDir() , TEXT("Media/PixelStreaming/Resources/WebServers/get_ps_servers.bat"));

    if ( FPaths::FileExists(BatFilePath) )
    {
        // 배치 파일 실행
        FPlatformProcess::CreateProc(*BatFilePath , nullptr , true , false , false , nullptr , 0 , nullptr , nullptr);
        UE_LOG(LogTemp , Log , TEXT("Started Pixel Streaming servers using %s") , *BatFilePath);
    }
    else
    {
        UE_LOG(LogTemp , Error , TEXT("Could not find the Pixel Streaming servers batch file: %s") , *BatFilePath);
        FMessageDialog::Open(EAppMsgType::Ok , FText::FromString(TEXT("Could not find the Pixel Streaming servers batch file.")));
    }
}

void APixelStreamingActor::StartPixelStreaming()
{
    // 먼저 서버를 시작
    StartServers();

    // IPixelStreamingModule을 가져옴
    IPixelStreamingModule* PixelStreamingModule = FModuleManager::Get().LoadModulePtr<IPixelStreamingModule>("PixelStreaming");

    if ( PixelStreamingModule )
    {
        FString StreamId = TEXT("YourStreamerID");
        MyStreamer = PixelStreamingModule->CreateStreamer(StreamId);

        if ( MyStreamer.IsValid() )
        {
            FString SignallingServerURL = TEXT("ws://127.0.0.1:10440");
            MyStreamer->SetSignallingServerURL(SignallingServerURL);

            // GameViewport에서 SViewport 가져오기
            UGameViewportClient* GameViewportClient = GEngine->GameViewport;

            if ( GameViewportClient && GameViewportClient->Viewport )
            {
                TSharedPtr<SViewport> GameViewportWidget = StaticCastSharedPtr<SViewport>(GameViewportClient->GetGameViewportWidget());
                if ( GameViewportWidget.IsValid() )
                {
                    MyStreamer->SetTargetViewport(GameViewportWidget);
                    UE_LOG(LogTemp , Log , TEXT("Streaming game viewport."));
                }
                else
                {
                    UE_LOG(LogTemp , Warning , TEXT("GameViewportWidget is not valid, streaming default target."));
                }
            }

            MyStreamer->StartStreaming();
        }
        else
        {
            UE_LOG(LogTemp , Error , TEXT("Failed to create streamer."));
        }
    }
}

    void APixelStreamingActor::StopPixelStreaming()
    {
        if ( MyStreamer.IsValid() )
        {
            MyStreamer->StopStreaming();
            MyStreamer.Reset();
        }
    }