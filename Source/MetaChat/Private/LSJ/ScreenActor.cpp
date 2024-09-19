// Fill out your copyright notice in the Description page of Project Settings.


#include "LSJ/ScreenActor.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"

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
#include "Kismet/KismetMathLibrary.h"
#include "LSJ/LSJMainWidget.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "Components/SceneCaptureComponent2D.h"
#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreaming/Public/IPixelStreamingStreamer.h"
#include "LSJ/MetaChatGameInstance.h"
#include "YWK/Recorderactor.h"
#include "HSB/CustomCharacter.h"
#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreaming/Public/PixelStreamingVideoInputRenderTarget.h"
// Sets default values
AScreenActor::AScreenActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    sceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("sceneComp"));
    SetRootComponent(sceneComp);

    // Plane Mesh 초기화
    WindowScreenPlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WindowScreenPlaneMesh"));
    //PlaneMesh->SetupAttachment(RootComponent);
    WindowScreenPlaneMesh->SetupAttachment(sceneComp);
	WindowScreenPlaneMesh->SetRelativeLocation(FVector(0,0,0));
	WindowScreenPlaneMesh->SetRelativeScale3D(FVector(3.00000, 10000, 1.000000));
    // 기본 Plane Mesh 설정
    static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMeshAsset(TEXT("/Engine/BasicShapes/Plane.Plane"));
    if (PlaneMeshAsset.Succeeded())
    {
        WindowScreenPlaneMesh->SetStaticMesh(PlaneMeshAsset.Object);
    }
    // 기본 머티리얼 설정
    static ConstructorHelpers::FObjectFinder<UMaterial> DefaultMaterial(TEXT("/Script/Engine.Material'/Game/XR_LSJ/WindowViewer/BasicShapeMaterial.BasicShapeMaterial'"));
    if (DefaultMaterial.Succeeded())
    {
        WindowScreenPlaneMesh->SetMaterial(0, DefaultMaterial.Object);
    }
	WindowScreenPlaneMesh->SetVisibility(false);



	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(RootComponent);
	SceneCapture->CaptureSource = SCS_FinalColorLDR;
	//SceneCapture->TextureTarget = RenderTarget;
}

void AScreenActor::UpdateTexture()
{
	// 일정 시간 간격으로 화면 캡처 수행
	static float TimeAccumulator = 0.0f;
	const float CaptureInterval = 0.1f; // 1초에 한 번 캡처
	TimeAccumulator += GetWorld()->GetDeltaSeconds();

	if (TimeAccumulator >= CaptureInterval)
	{
		TimeAccumulator = 0.0f;
		FScopeLock Lock(&CriticalSection);
		if (CapturedTexture)
		{
			CapturedTexture->ConditionalBeginDestroy();
		}
		CapturedTexture = CaptureScreenToTexture();
        
		if (DynamicMaterial && CapturedTexture&& WindowScreenPlaneMesh)
		{
			// BaseTexture 파라미터에 텍스처 설정
			DynamicMaterial->SetTextureParameterValue(TEXT("Base"), CapturedTexture);
		}
	}
}
UTexture2D* AScreenActor::CaptureScreenToTexture()
{
	int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	HDC hScreenDC = GetDC(NULL);
	HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, ScreenWidth, ScreenHeight);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);
	BitBlt(hMemoryDC, 0, 0, ScreenWidth, ScreenHeight, hScreenDC, 0, 0, SRCCOPY);
	SelectObject(hMemoryDC, hOldBitmap);

	BITMAPINFOHEADER bi;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = ScreenWidth;
	bi.biHeight = -ScreenHeight; // 비트맵이 상하 반전되지 않도록 음수로 설정
	bi.biPlanes = 1;
	bi.biBitCount = 32; // 32비트 비트맵 (8비트 * 4 채널)
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	std::vector<BYTE> Buffer(ScreenWidth * ScreenHeight * 4);
	GetDIBits(hMemoryDC, hBitmap, 0, ScreenHeight, Buffer.data(), (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	 // 명도 감소 비율 (예: 0.8로 명도를 20% 감소)
    float BrightnessFactor = 0.8f;

    // 각 픽셀의 RGB 값을 명도 감소 비율로 조정
    for (int32 i = 0; i < ScreenWidth * ScreenHeight * 4; i += 4)
    {
        // R, G, B 값을 감소
        Buffer[i] = static_cast<BYTE>(Buffer[i] * BrightnessFactor);     // Red
        Buffer[i + 1] = static_cast<BYTE>(Buffer[i + 1] * BrightnessFactor); // Green
        Buffer[i + 2] = static_cast<BYTE>(Buffer[i + 2] * BrightnessFactor); // Blue
        // 알파 값(Buffer[i+3])은 변경하지 않음 (투명도 유지)
    }

	// UTexture2D 동적 생성
	UTexture2D* Texture = UTexture2D::CreateTransient(ScreenWidth, ScreenHeight, PF_B8G8R8A8);
	if (!Texture)
		return nullptr;

	// 텍스처 데이터를 업데이트
	void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, Buffer.data(), Buffer.size());
	Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
	Texture->UpdateResource();

	// 리소스 정리
	DeleteObject(hBitmap);
	DeleteDC(hMemoryDC);
	ReleaseDC(NULL, hScreenDC);

	/*	FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
		FTextureResource* TextureResource = Texture->GetResource();		*/

	return Texture;
    //int32 ScreenWidth =  GetSystemMetrics(SM_CXSCREEN);
    //int32 ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    //// 화면의 HDC를 얻기
    //HDC hScreenDC = GetDC(NULL);
    //HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    //HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, ScreenWidth, ScreenHeight);
    //HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);

    //// 화면 캡처
    //BitBlt(hMemoryDC, 0, 0, ScreenWidth, ScreenHeight, hScreenDC, 0, 0, SRCCOPY);
    //SelectObject(hMemoryDC, hOldBitmap);

    //// BITMAPINFOHEADER 설정
    //BITMAPINFOHEADER bi;
    //bi.biSize = sizeof(BITMAPINFOHEADER);
    //bi.biWidth = ScreenWidth;
    //bi.biHeight = -ScreenHeight;  // 이미지를 뒤집기 위해 음수로 설정
    //bi.biPlanes = 1;
    //bi.biBitCount = 32;  // 32비트 비트맵 (RGBA)
    //bi.biCompression = BI_RGB;
    //bi.biSizeImage = 0;
    //bi.biXPelsPerMeter = 0;
    //bi.biYPelsPerMeter = 0;
    //bi.biClrUsed = 0;
    //bi.biClrImportant = 0;

    //// 비트맵 데이터를 저장할 버퍼 생성
    //std::vector<BYTE> Buffer(ScreenWidth * ScreenHeight * 4);
    //GetDIBits(hMemoryDC, hBitmap, 0, ScreenHeight, Buffer.data(), (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    //// UTexture2D 동적 생성
    //UTexture2D* Texture = UTexture2D::CreateTransient(ScreenWidth, ScreenHeight, PF_B8G8R8A8);
    //if (!Texture)
    //{
    //    DeleteObject(hBitmap);
    //    DeleteDC(hMemoryDC);
    //    ReleaseDC(NULL, hScreenDC);
    //    return nullptr;
    //}

    //// 텍스처 데이터를 업데이트
    //void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
    //FMemory::Memcpy(TextureData, Buffer.data(), Buffer.size());
    //Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
    //Texture->UpdateResource();

    //// 리소스 해제
    //DeleteObject(hBitmap);
    //DeleteDC(hMemoryDC);
    //ReleaseDC(NULL, hScreenDC);

    //return Texture;  // 캡처된 모니터 화면을 텍스처로 반환
}

void AScreenActor::StopLookSharingScreenWidget()
{
	ViewSharingUserStreamID = "";
	StopLookSharingScreen();
	MainWidget->LookStreaming(false);
}

void AScreenActor::StopLookSharingScreen()
{
	// 블루프린트 함수 이름
	FName FunctionName(TEXT("StopLookPixelStreaming")); // 블루프린트에서 정의한 함수명

	// 블루프린트 함수 가져오기
	UFunction* Function = FindFunction(FunctionName);

	if (Function)
	{
		// 블루프린트 함수 호출 (매개변수가 없는 경우)
		ProcessEvent(Function, nullptr);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Function not found: %s"), *FunctionName.ToString());
	}
}

void AScreenActor::BeginLookSharingScreen()
{
	// 블루프린트 함수 이름
	FName FunctionName(TEXT("BeginLookPixelStreaming")); // 블루프린트에서 정의한 함수명

	// 블루프린트 함수 가져오기
	UFunction* Function = FindFunction(FunctionName);

	if (Function)
	{
		// 블루프린트 함수 호출 (매개변수가 없는 경우)
		ProcessEvent(Function, nullptr);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Function not found: %s"), *FunctionName.ToString());
	}
}
void AScreenActor::ChangeLookSharingScreen()
{
	// 블루프린트 함수 이름
	FName FunctionName(TEXT("ChangeLookPixelStreaming")); // 블루프린트에서 정의한 함수명

	// 블루프린트 함수 가져오기
	UFunction* Function = FindFunction(FunctionName);

	if (Function)
	{
		// 블루프린트 함수 호출 (매개변수가 없는 경우)
		ProcessEvent(Function, nullptr);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Function not found: %s"), *FunctionName.ToString());
	}
}

// Called when the game starts or when spawned
void AScreenActor::BeginPlay()
{
	Super::BeginPlay();
	APawn* playerPawn =UGameplayStatics::GetPlayerPawn(GetWorld(),0);
	UCameraComponent* playerCamera = playerPawn->GetComponentByClass<UCameraComponent>();
	//WindowScreenPlaneMesh->SetRelativeLocation(FVector(200,0,10));
	WindowScreenPlaneMesh->SetRelativeScale3D(FVector(3,2,1));
	sceneComp->AttachToComponent(playerCamera,FAttachmentTransformRules::SnapToTargetIncludingScale);

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(sceneComp->GetComponentLocation(), playerCamera->GetComponentLocation());

	// Z 축이 카메라를 향하도록 회전
	//WindowScreenPlaneMesh->SetRelativeRotation(FRotator(0, LookAtRotation.Yaw, LookAtRotation.Roll));
	DynamicMaterial = UMaterialInstanceDynamic::Create(WindowScreenPlaneMesh->GetMaterial(0), this);
	WindowScreenPlaneMesh->SetMaterial(0, DynamicMaterial);
	MainWidget = Cast<ULSJMainWidget>(CreateWidget<UUserWidget>(GetWorld(),MainWidgetFactory));
	if (MainWidget)
	{
		MainWidget->AddToViewport(-1);
		MainWidget->SetScreenActor(this);
	}

	auto* gi = Cast<UMetaChatGameInstance>(GetWorld()->GetGameInstance());
	UserID = gi->UserID;
	//UE_LOG(LogTemp,Error,TEXT("UMetaChatGameInstance : WorldID %d"),gi->WorldID);

	RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	RenderTarget->CompressionSettings = TextureCompressionSettings::TC_Default;
	RenderTarget->SRGB = false;
	RenderTarget->bAutoGenerateMips = false;
	RenderTarget->bForceLinearGamma = true;
	RenderTarget->TargetGamma = 2.2f;
	RenderTarget->AddressX = TextureAddress::TA_Clamp;
	RenderTarget->AddressY = TextureAddress::TA_Clamp;
	RenderTarget->InitAutoFormat(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

	if (RenderTarget && SceneCapture)
    {
        SceneCapture->TextureTarget = RenderTarget;
        //SceneCapture->CaptureScene();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Initialization failed in BeginPlay"));
    }
}

// Called every frame
void AScreenActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AScreenActor::UserStartStreaming(TArray<FString> Users)
{
//스트리밍 중이고 UserStreamID가 정해지지 않았다면
	if (MainWidget->Streaming() && UserStreamID.IsEmpty())
	{
		int32 currentNum = 0;
		bool bEditor = false;
		for(FString ID : Users)
		{	
			if(false==ID.Contains(TEXT("Editor"), ESearchCase::CaseSensitive))
				continue;
			TArray<FString> Numstrings;
			ID.ParseIntoArray(Numstrings,TEXT("Editor"),false);

			if (Numstrings[1].Equals(""))
			{
				bEditor = true;
			}
			else if (currentNum != FCString::Atoi(*Numstrings[1]))
			{
				if(FCString::Atoi(*Numstrings[1]) == 1)
					UserStreamID = "Editor";
				else
					UserStreamID = "Editor" + FString::FromInt(currentNum);
				break;
			}
			currentNum++;
			//UE_LOG(LogTemp,Error,TEXT("Numstrings : %s"),*Numstrings[1]);
		}

		if(UserStreamID == "" && false==bEditor)
			UserStreamID = "Editor";
		else if(UserStreamID == "" && true==bEditor)
			UserStreamID = "Editor" + FString::FromInt(currentNum);
		
		MainWidget->CurrentStreamer->StartStreaming();

		//캐릭터의 AreaActor 안에 UserStreamingInfo의 StreamID를 수정한다.
		
		ACustomCharacter* player = Cast<ACustomCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (nullptr == player)
			return;
		if (nullptr == player->AreaActor)
			return;
		player->ServerUpdateUserInfoToRecordActor(player->AreaActor,UserID,UserStreamID);
	}
}

FString AScreenActor::GetSharingUsers(TArray<FString> Users)
{
	//스트리밍 중이고 UserStreamID가 정해지지 않았다면
	if (MainWidget->Streaming() && UserStreamID.IsEmpty())
	{
		int32 currentNum = 0;
		bool bEditor = false;
		for(FString ID : Users)
		{	
			if(false==ID.Contains(TEXT("Editor"), ESearchCase::CaseSensitive))
				continue;
			TArray<FString> Numstrings;
			ID.ParseIntoArray(Numstrings,TEXT("Editor"),false);

			if (Numstrings[1].Equals(""))
			{
				bEditor = true;
			}
			else if (currentNum != FCString::Atoi(*Numstrings[1]))
			{
				if(FCString::Atoi(*Numstrings[1]) == 1)
					UserStreamID = "Editor";
				else
					UserStreamID = "Editor" + FString::FromInt(currentNum);
				break;
			}
			currentNum++;
			//UE_LOG(LogTemp,Error,TEXT("Numstrings : %s"),*Numstrings[1]);
		}

		if(UserStreamID == "" && false==bEditor)
			UserStreamID = "Editor";
		else if(UserStreamID == "" && true==bEditor)
			UserStreamID = "Editor" + FString::FromInt(currentNum);
		
		MainWidget->CurrentStreamer->StartStreaming();

		//캐릭터의 AreaActor 안에 UserStreamingInfo의 StreamID를 수정한다.
		
		ACustomCharacter* player = Cast<ACustomCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (nullptr == player)
			return "";
		if (nullptr == player->AreaActor)
			return "";
		player->ServerUpdateUserInfoToRecordActor(player->AreaActor,UserID,UserStreamID);
		//TArray<FString> Items;
		//MainWidget->InitSlot(Items);
		return "";
	}

	if (MainWidget)
	{
		
		MainWidget->InitSlot(Users);
	}
		
	if(Users.Num()>0)
		return Users[0];
	else
		return "";
}

void AScreenActor::SetViewSharingUserID(FString ID)
{
	ViewSharingUserStreamID = ID;
	ChangeLookSharingScreen();
}

bool AScreenActor::GetLookStreaming()
{
	return MainWidget->LookStreaming();
}