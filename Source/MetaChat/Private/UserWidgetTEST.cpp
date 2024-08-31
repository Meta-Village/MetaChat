// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidgetTEST.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "MetaChatGameInstance.h"
#include "JsonParseLib.h"
#include "Components/TextBlock.h"
#include "HttpActor.h"

//window api
#include "Engine/Texture2D.h"
#include "Engine/Texture2DDynamic.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#include <vector> 
#include "Styling/SlateBrush.h"
#include "Components/Image.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

void UUserWidgetTEST::NativeConstruct()
{
	Super::NativeConstruct();
	//버튼 연결
	//ButtonTest->OnClicked.AddDynamic(this,&UUserWidgetTEST::TestButtonCallback);
	//ButtonJoin->OnClicked.AddDynamic(this, &UUserWidgetTEST::JoinCallback);
	HttpGetButton->OnClicked.AddDynamic(this, &UUserWidgetTEST::OnMyClickSendGet);
	HttpPostButton->OnClicked.AddDynamic(this, &UUserWidgetTEST::OnMyClickSendPost);
}
void UUserWidgetTEST::NativeDestruct()
{
	bIsTaskCancelled = true;  // 종료 시 플래그 설정

	if ( AsyncTaskHandle.IsValid() && !AsyncTaskHandle.IsReady() )
	{
		AsyncTaskHandle.Wait();  // 필요시 Task가 종료되도록 대기
	}

	Super::NativeDestruct();  // 부모 클래스의 정리 작업 호출
}

void UUserWidgetTEST::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}
void UUserWidgetTEST::SetTextLog(FString log)
{
	TextLog->SetText(FText::FromString(log));
}

void UUserWidgetTEST::SetImageTexture(class UTexture2D* Texture)
{
	if ( ImageWidget && Texture )
	{
		FSlateBrush Brush = ImageWidget->GetBrush();  // 현재 Brush 가져오기
		Brush.SetResourceObject(Texture);  // Brush에 텍스처 설정
		ImageWidget->SetBrush(Brush);  // Brush를 다시 설정
	}
}

UTexture2D* UUserWidgetTEST::GetImageTexture()
{
	if ( ImageWidget )
	{
		const FSlateBrush& Brush = ImageWidget->GetBrush();  // Brush를 안전하게 가져옴
		if ( UObject* ResourceObject = Brush.GetResourceObject() )
		{
			return Cast<UTexture2D>(ResourceObject);  // 텍스처 캐스팅
		}
	}
	return nullptr;
}

void UUserWidgetTEST::SetViewer(AActor* actor)
{
	windowViewer = actor;
	PlaneMesh = windowViewer->GetComponentByClass<UStaticMeshComponent>();
	DynamicMaterial = UMaterialInstanceDynamic::Create(PlaneMesh->GetMaterial(0) , this);
}

void UUserWidgetTEST::UpdateWidgetTexture()
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

			try
			{
				CapturedTexture = CaptureScreenToTexture();
			}
			catch ( ... )
			{
				// 캡처 중 문제가 발생했을 때의 처리
				return;
			}

			// 메인 스레드에서 UI 업데이트
			AsyncTask(ENamedThreads::GameThread , [this , CapturedTexture]()
			{
						if ( CapturedTexture && !bIsTaskCancelled && IsValid(this) && IsValid(ImageWidget) )
						{
							UTexture2D* OldTexture = GetImageTexture();
							if ( OldTexture )
							{
								OldTexture->ConditionalBeginDestroy();
							}

							try
							{
								SetImageTexture(CapturedTexture);
							}
							catch ( ... )
							{
								// 캡처 중 문제가 발생했을 때의 처리
								return;
							}
						

							if ( DynamicMaterial )
							{
								CapturedTexture->SRGB = true;
								// BaseTexture 파라미터에 텍스처 설정
								DynamicMaterial->SetTextureParameterValue(TEXT("Base") , CapturedTexture);

								// PlaneMesh에 머티리얼 적용
								PlaneMesh->SetMaterial(0 , DynamicMaterial);
								UE_LOG(LogTemp , Warning , TEXT("CapturedTexture is valid and applied."));
							}
							
						}
			});
		});
	}
}

UTexture2D* UUserWidgetTEST::CaptureScreenToTexture()
{
	int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	HDC hScreenDC = GetDC(NULL);
	HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC , ScreenWidth , ScreenHeight);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC , hBitmap);
	BitBlt(hMemoryDC , 0 , 0 , ScreenWidth , ScreenHeight , hScreenDC , 0 , 0 , SRCCOPY);
	SelectObject(hMemoryDC , hOldBitmap);

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
	GetDIBits(hMemoryDC , hBitmap , 0 , ScreenHeight , Buffer.data() , (BITMAPINFO*)&bi , DIB_RGB_COLORS);

	// UTexture2D 동적 생성
	UTexture2D* Texture = UTexture2D::CreateTransient(ScreenWidth , ScreenHeight , PF_B8G8R8A8);
	if ( !Texture )
		return nullptr;

	// 텍스처 데이터를 업데이트
	void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData , Buffer.data() , Buffer.size());
	Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
	Texture->UpdateResource();

	// 리소스 정리
	DeleteObject(hBitmap);
	DeleteDC(hMemoryDC);
	ReleaseDC(NULL , hScreenDC);

	return Texture;
}
void UUserWidgetTEST::TestButtonCallback()
{
	UMetaChatGameInstance* gi = Cast<UMetaChatGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	gi->CreateGameSession();
}
void UUserWidgetTEST::JoinCallback()
{
	UMetaChatGameInstance* gi = Cast<UMetaChatGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	gi->JoinGameSession();
}

void UUserWidgetTEST::OnMyClickSendGet()
{
	//httpActor에게 공공데이터값을 받아오라고 요청 하고 싶다.
	//FString fullURL = FString::Printf(TEXT("%s?serviceKey=%s&pageNo=%s&numOfRows=%s"), *url, *key, *EditPageNo->GetText().ToString(), *EditNumOfRows->GetText().ToString());
	FString fullURL = "http://master-of-prediction.shop:8123/test";
	HttpActor->RsqGetTest(fullURL);
}

void UUserWidgetTEST::OnMyClickSendPost()
{
	TMap<FString, FString> studentData;
	studentData.Add("Name", "Kei");
	studentData.Add("Age", "23");
	studentData.Add("Height", "185");
	FString json = UJsonParseLib::MakeJson(studentData);
	FString fullURL = "http://master-of-prediction.shop:8123/test";
	HttpActor->RsqPostTest(fullURL, json);
}

void UUserWidgetTEST::SetHttpActor(AHttpActor* actor)
{
	HttpActor = actor;
}