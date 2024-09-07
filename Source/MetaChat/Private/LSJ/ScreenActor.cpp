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
			CapturedTexture->SRGB = true;
			// BaseTexture 파라미터에 텍스처 설정
			DynamicMaterial->SetTextureParameterValue(TEXT("Base"), CapturedTexture);

			// PlaneMesh에 머티리얼 적용
			
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

	return Texture;
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
void AScreenActor::BeginStreaming()
{
	// 블루프린트 함수 이름
	FName FunctionName(TEXT("NewFunction")); // 블루프린트에서 정의한 함수명

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
}

// Called every frame
void AScreenActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FString AScreenActor::GetSharingUsers(TArray<FString> Users)
{
	if(MainWidget)
		MainWidget->InitSlot(Users);
	if(Users.Num()>0)
		return Users[0];
	else
		return "";
}
void AScreenActor::SetViewSharingUserID(FString ID)
{
	UserID = ID;
	ChangeLookSharingScreen();
}