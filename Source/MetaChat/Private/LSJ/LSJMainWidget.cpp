
// Fill out your copyright notice in the Description page of Project Settings.


#include "LSJ/LSJMainWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "LSJ/ScreenActor.h"
#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreaming/Public/IPixelStreamingStreamer.h"
#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreaming/Public/PixelStreamingVideoInputBackBuffer.h"
#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreaming/Public/IPixelStreamingModule.h"
#include "Modules/ModuleManager.h"

#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSubsystem.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSessionSettings.h"

#include "LSJ/SharingUserSlot.h"
#include "Engine/TextureRenderTarget2D.h"
//#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreaming/Public/PixelStreamingVideoInputRenderTarget.h"
//#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreamingBlueprint/Internal/PixelStreamingStreamerVideoInputRenderTarget.h"
//#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreamingBlueprint/Internal/PixelStreamingStreamerVideoInput.h"
#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreaming/Internal/PixelStreamingVideoInputMediaCapture.h"
#include "../../../../Plugins/Media/MediaIOFramework/Source/MediaIOCore/Public/MediaOutput.h"
#include "../../../../Plugins/Media/MediaIOFramework/Source/MediaIOCore/Public/MediaCapture.h"

#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreaming/Public/PixelStreamingVideoInputRenderTarget.h"
#include "Components/SceneCaptureComponent2D.h"
//#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreamingEditor/Public/PixelStreamingVideoInputViewport.h"
#include "HSB/CustomCharacter.h"
#include "YWK/Recorderactor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PawnMovementComponent.h"



void ULSJMainWidget::SetUserID(FString ID)
{
	ScreenActor->SetViewSharingUserID(ID);
}
void ULSJMainWidget::ClickSlot(FString ID,bool bClick)
{
	//시그널 서버와 처음부터 연결

	//Button의 텍스트를 ID표시
	//ScreenActor->ViewSharingUserStreamID는 StreamID로 표시
	ACustomCharacter* CustomPlayer = Cast<ACustomCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
	if(nullptr==CustomPlayer)
		return;
	if(nullptr==CustomPlayer->AreaActor)
		return;
	ARecorderactor* AreaActor = Cast<ARecorderactor>(CustomPlayer->AreaActor);
	if (AreaActor)
	{
		if(AreaActor->UserStreamingInfo.Num()<=0)
			return;
		for (FUserStreamingInfo& IDInfo : AreaActor->UserStreamingInfo)
		{
			if (IDInfo.UserID.Equals(ID))
			{
				IDInfo.bClicked = !(IDInfo.bClicked);
				if (IDInfo.bClicked)
				{
					ScreenActor->ViewSharingUserStreamID = IDInfo.UserStreamID;
					//ScreenActor->BeginLookSharingScreen();
					ScreenActor->ChangeLookSharingScreen();
					ImageSharingScreen->SetVisibility(ESlateVisibility::Visible);
					LookStreaming(true);
				}
				else
				{
					//보기 종료
					ScreenActor->StopLookSharingScreenWidget();
					IDInfo.bClicked = false;
					/*ScreenActor->ViewSharingUserStreamID = "";
					ScreenActor->StopLookSharingScreen();
					ImageSharingScreen->SetVisibility(ESlateVisibility::Hidden);
					LookStreaming(false);*/
				}
			}
			else
				IDInfo.bClicked = false;
		}
	}
	//ChangeLookSharingScreen()로 StreamID로 스트림 화면 전환
	//bClick값에 따라 ImageSharingScreen 켜지고 꺼지고

	//bClick을 키면 현재 키를 제외한 다른 모든 버튼의 bClick값을 false로 변경
	
}
void ULSJMainWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if(CurrentStreamer)
		CurrentStreamer->SetVideoInput(nullptr);

	TextureSharingIdle->RemoveFromRoot();
	TextureSharingClicked->RemoveFromRoot();
	TextureIdle->RemoveFromRoot();
	TextureClicked->RemoveFromRoot();
}
void ULSJMainWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	TextureSharingIdle = LoadObject<UTexture2D>(nullptr, TEXT("/Game/XR_LSJ/Image/Group_25__1_"));
    TextureSharingClicked = LoadObject<UTexture2D>(nullptr, TEXT("/Game/XR_LSJ/Image/Group_23"));
    TextureIdle = LoadObject<UTexture2D>(nullptr, TEXT("/Game/XR_LSJ/Image/Group_5"));
	TextureClicked = LoadObject<UTexture2D>(nullptr, TEXT("/Game/XR_LSJ/Image/Group_38.Group_38"));

	TextureSharingIdle->AddToRoot();
	TextureSharingClicked->AddToRoot();
	TextureIdle->AddToRoot();
	TextureClicked->AddToRoot();
}
void ULSJMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry,InDeltaTime);
	if (Streaming() && nullptr != ScreenActor)
	{
		ScreenActor->UpdateTexture();
	}

		// 현재 플레이어 캐릭터 가져오기
	ACustomCharacter* PlayerCharacter = Cast<ACustomCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter)
	{
		// 현재 플레이어가 있는 존
		FString CurrentZoneName = PlayerCharacter->GetCurrentZoneName();

		// 존이 변경되었을 때만 로그 출력
		if (CurrentZoneName != PreviousZoneName)
		{
			UE_LOG(LogTemp, Log, TEXT("Current Zone: %s"), *CurrentZoneName);
			PreviousZoneName = CurrentZoneName;  // 이전 존 업데이트
		}

		// 존이 ROOM1 또는 ROOM2이면 위젯을 보이게 설정
		if (CurrentZoneName == "ROOM1" || CurrentZoneName == "ROOM2" || CurrentZoneName == "ROOM3" || CurrentZoneName == "ROOM4")
		{
			VisibleSwitcher(true);
			PreviousZoneActor = PlayerCharacter->AreaActor;
		}
		else
		{
			//스트리밍 중지
			if (Streaming())
			{
				OnButtonWindowScreen();
			}
			//스트리밍 보기 중지
			ARecorderactor* AreaActor = Cast<ARecorderactor>(PreviousZoneActor);
			if (AreaActor)
				for (FUserStreamingInfo& IDInfo : AreaActor->UserStreamingInfo)
				{
					IDInfo.bClicked = false;
				}
			ScreenActor->ViewSharingUserStreamID = "";
			ScreenActor->StopLookSharingScreen();
			LookStreaming(false);
			//스트리밍 보기 버튼 감추기
			if(SharingUserPanel)
				SharingUserPanel->ClearChildren();
			VisibleSwitcher(false);
		}
	}
}
void ULSJMainWidget::SetImageTexture(class UTexture2D* Texture)
{
	if ( ImageWindowScreen && Texture )
	{
		FSlateBrush Brush = ImageWindowScreen->GetBrush();  // 현재 Brush 가져오기
		Brush.SetResourceObject(Texture);  // Brush에 텍스처 설정
		ImageWindowScreen->SetBrush(Brush);  // Brush를 다시 설정
	}
}
UTexture2D* ULSJMainWidget::GetImageTexture()
{
	if ( ImageWindowScreen )
	{
		const FSlateBrush& Brush = ImageWindowScreen->GetBrush();  // Brush를 안전하게 가져옴
		if ( UObject* ResourceObject = Brush.GetResourceObject() )
		{
			return Cast<UTexture2D>(ResourceObject);  // 텍스처 캐스팅
		}
	}
	return nullptr;
}
void ULSJMainWidget::VisibleSwitcher(bool bIsVisible)
{
	if (ButtonWindowScreen)
	{
		// 보여야 할 때
		if (bIsVisible)
		{
			//보이기
			ButtonWindowScreen->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			// 숨기기
			ButtonWindowScreen->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void ULSJMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
	ButtonLookSharingScreen->OnClicked.AddDynamic(this,&ULSJMainWidget::OnButtonLookSharingScreen);
	ButtonWindowScreen->OnClicked.AddDynamic(this, &ULSJMainWidget::OnButtonWindowScreen);
	ImageSharingScreen->SetVisibility(ESlateVisibility::Hidden);
	ImageCoveringScreen->SetVisibility(ESlateVisibility::Hidden);
	ButtonLookSharingScreen->SetVisibility(ESlateVisibility::Hidden);
}
// 버튼 스타일의 이미지를 변경하는 함수
void ULSJMainWidget::SetButtonStyle(UButton* Button, UTexture2D* NormalTexture, UTexture2D* PressedTexture, UTexture2D* HoveredTexture)
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
void ULSJMainWidget::OnButtonWindowScreen()
{
	Streaming(!Streaming());
	FString streamID = "Editor";
	if (Streaming())
	{
		//캐릭터 가만히 있기
		ACustomCharacter* player = Cast<ACustomCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (nullptr == player)
			return;
		player->GetMovementComponent()->Deactivate();
		//TextWindowScreen->SetText(FText::FromString(TEXT("공유중")));
		//ImageWindowScreen->SetVisibility(ESlateVisibility::Visible);
		//ScreenActor->WindowScreenPlaneMesh->SetVisibility(true);
		//ScreenActor->BeginStreaming();
		ScreenActor->PostProcessVolume->BlendWeight = 1.0f;
		// 1. PixelStreaming 모듈을 가져옵니다.
		if (GIsEditor)
		{
			// 에디터에서 실행 중일 때
			UE_LOG(LogTemp, Warning, TEXT("Running in Editor!"));

			IPixelStreamingModule* PixelStreamingModule = FModuleManager::Get().LoadModulePtr<IPixelStreamingModule>("PixelStreaming");

			if (PixelStreamingModule)
			{
				// 현재 세션의 아이디를 가져와서 Streamer를 생성한다.
				CurrentStreamer = PixelStreamingModule->FindStreamer(streamID);//GetCurrentSessionID());
				//TSharedPtr<IPixelStreamingStreamer> Streamer = PixelStreamingModule->CreateStreamer("Test01");
				if (CurrentStreamer.IsValid())
				{
					SetButtonStyle(ButtonWindowScreen,TextureSharingClicked,TextureSharingClicked,TextureSharingClicked);
				
					//TSharedPtr<FPixelStreamingVideoInputBackBuffer> VideoInput = FPixelStreamingVideoInputBackBuffer::Create();
					//Back Buffer를 비디오 입력으로 설정합니다.
					CurrentStreamer->SetInputHandlerType(EPixelStreamingInputType::RouteToWidget);

					
					UGameViewportClient* GameViewport = GEngine->GameViewport;
					ScreenActor->SceneCapture->Activate();
	
					
	// 2. Pixel Streaming 비디오 입력으로 설정
						VideoInput = FPixelStreamingVideoInputRenderTarget::Create(ScreenActor->SceneCapture->TextureTarget);

						CurrentStreamer->SetVideoInput(VideoInput); // 스트리밍에 사용
	
						//Streamer->SetVideoInput(FPixelStreamingVideoInputViewport::Create(Streamer));
						CurrentStreamer->SetSignallingServerURL("ws://master-of-prediction.shop:8890");
					
						//시그널 서버에 연결하고 StreamID 목록에서 비어있거나 추가할 StreamID를 탐색한다. 탐색 후 그 StreamID를 UserStreamID로 저장한다.
						ScreenActor->BeginLookSharingScreen();
					
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Could not find a valid streamer with the given ID."));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("PixelStreamingModule is not available."));
			}
		}
		else
		{
			// 패키징된 게임에서 실행 중일 때
			UE_LOG(LogTemp, Warning, TEXT("Running in Packaged Game!"));

			IPixelStreamingModule& PixelStreamingModule1 = FModuleManager::LoadModuleChecked<IPixelStreamingModule>("PixelStreaming");
			CurrentStreamer = PixelStreamingModule1.CreateStreamer(streamID);
			
			SetButtonStyle(ButtonWindowScreen,TextureSharingClicked,TextureSharingClicked,TextureSharingClicked);
				
			//TSharedPtr<FPixelStreamingVideoInputBackBuffer> VideoInput = FPixelStreamingVideoInputBackBuffer::Create();
			//Back Buffer를 비디오 입력으로 설정합니다.
			CurrentStreamer->SetInputHandlerType(EPixelStreamingInputType::RouteToWidget);

					
			UGameViewportClient* GameViewport = GEngine->GameViewport;
			ScreenActor->SceneCapture->Activate();
	
					
// 2. Pixel Streaming 비디오 입력으로 설정
			VideoInput = FPixelStreamingVideoInputRenderTarget::Create(ScreenActor->SceneCapture->TextureTarget);

			CurrentStreamer->SetVideoInput(VideoInput); // 스트리밍에 사용
	
			//Streamer->SetVideoInput(FPixelStreamingVideoInputViewport::Create(Streamer));
			CurrentStreamer->SetSignallingServerURL("ws://master-of-prediction.shop:8890");
					
			//시그널 서버에 연결하고 StreamID 목록에서 비어있거나 추가할 StreamID를 탐색한다. 탐색 후 그 StreamID를 UserStreamID로 저장한다.
			ScreenActor->BeginLookSharingScreen();
		}
	}
	else //스트리밍 종료
	{
		ScreenActor->UserStreamID = "";
		//TextWindowScreen->SetText(FText::FromString(TEXT("화면공유")));
		//ScreenActor->WindowScreenPlaneMesh->SetVisibility(false);
		ScreenActor->PostProcessVolume->BlendWeight = 0.0f;
		ImageWindowScreen->SetVisibility(ESlateVisibility::Hidden);
		if (CurrentStreamer.IsValid())
		{
			SetButtonStyle(ButtonWindowScreen,TextureSharingIdle,TextureSharingIdle,TextureSharingIdle);

			// 4. 스트리밍을 시작합니다.
			CurrentStreamer->StopStreaming();

			ACustomCharacter* player = Cast<ACustomCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
			if (nullptr == player)
				return;
			if (nullptr == player->AreaActor)
				return;
			player->GetMovementComponent()->Activate();
			player->ServerUpdateUserInfoToRecordActor(player->AreaActor, ScreenActor->UserID, "");
		}
		//// 1. PixelStreaming 모듈을 가져옵니다.
		//IPixelStreamingModule* PixelStreamingModule = FModuleManager::GetModulePtr<IPixelStreamingModule>("PixelStreaming");

		//if (PixelStreamingModule)
		//{
		//	// 2. 스트리머를 가져옵니다.
		//	TSharedPtr<IPixelStreamingStreamer> Streamer = PixelStreamingModule->FindStreamer(streamID);

		//	
		//	else
		//	{
		//		UE_LOG(LogTemp, Error, TEXT("Could not find a valid streamer with the given ID."));
		//	}
		//}
		//else
		//{
		//	UE_LOG(LogTemp, Error, TEXT("PixelStreamingModule is not available."));
		//}
	}
}

void ULSJMainWidget::OnButtonLookSharingScreen()
{
	LookStreaming(!LookStreaming());
	if (LookStreaming())
	{
		TextLookSharingScreen->SetText(FText::FromString(TEXT("보는중")));
		ImageSharingScreen->SetVisibility(ESlateVisibility::Visible);
		//블루프린트 subs
		ScreenActor->BeginLookSharingScreen();
		//ImageCoveringScreen->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		TextLookSharingScreen->SetText(FText::FromString(TEXT("화면보기")));
		ImageSharingScreen->SetVisibility(ESlateVisibility::Hidden);
		//블루프린트 subs
		ScreenActor->StopLookSharingScreen();
		SharingUserPanel->ClearChildren();
		ImageCoveringScreen->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ULSJMainWidget::SetScreenActor(AScreenActor* Actor)
{
	ScreenActor = Actor;
}

FString ULSJMainWidget::GetCurrentSessionID()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			// "GameSession"은 기본 세션 이름이며, 필요에 따라 다른 세션 이름을 사용할 수 있음
			FNamedOnlineSession* NamedSession = SessionInterface->GetNamedSession(NAME_GameSession);
			if (NamedSession)
			{
				// 세션 ID 가져오기
				return NamedSession->GetSessionIdStr();
			}
		}
	}

	// 세션이 없거나 가져오지 못했을 때
	return FString("No Session Found");
}

void ULSJMainWidget::InitSlot(TArray<FString> Items)
{
    // 기존 슬롯 제거
    SharingUserPanel->ClearChildren();
    int32 Row = 0;
    int32 Column = 0;

	ACustomCharacter* CustomPlayer = Cast<ACustomCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
	if(nullptr==CustomPlayer)
		return;
	if(nullptr==CustomPlayer->AreaActor)
		return;
	ARecorderactor* AreaActor = Cast<ARecorderactor>(CustomPlayer->AreaActor);
	if (AreaActor)
	{
		if(AreaActor->UserStreamingInfo.Num()<=0)
			return;
		for (FUserStreamingInfo IDInfo : AreaActor->UserStreamingInfo)
		{
			if(IDInfo.UserStreamID.IsEmpty())
				continue;

			SharingUserSlot = CastChecked<USharingUserSlot>(CreateWidget(GetWorld(), SharingUserSlotFactory));
			if (SharingUserSlot)
			{
				// 슬롯 가시성 및 레이아웃 확인
				SharingUserSlot->SetVisibility(ESlateVisibility::Visible);
				SharingUserSlot->SetUserID(IDInfo.UserID);
				if(IDInfo.bClicked)
					SetButtonStyle(SharingUserSlot->ViewButton,TextureClicked,TextureClicked,TextureClicked);
				else
					SetButtonStyle(SharingUserSlot->ViewButton,TextureIdle,TextureIdle,TextureIdle);
				//SharingUserSlot->FUserIDButtonDelegate_OneParam.BindUFunction(this,FName("SetUserID"));
				SharingUserSlot->UserIDButtonDelegate_TwoParams.BindUFunction(this,FName("ClickSlot"));
	
				// Grid 에 슬롯 추가
				SharingUserPanel->AddChildToUniformGrid(SharingUserSlot,Row, 0);
				
				// Row 값 증가
				Row++;

				if (!SharingUserPanel)
				{
					UE_LOG(LogTemp, Error, TEXT("PartsPanel is not valid."));
					return;
				}

				//SharingUserSlot->clickcnt = P_clickcnt; // 클릭 값 전달 (계속 InvSlot 갱신돼서 clickcnt값 업데이트 안 되는 문제 때문)
			}
		}
	}
 //   // 아이템 데이터 바탕으로 슬롯 생성 및 추가
 //   for(FString UserStreamID : Items)
	//{
 //       SharingUserSlot = CastChecked<USharingUserSlot>(CreateWidget(GetWorld(), SharingUserSlotFactory));
 //       if (SharingUserSlot)
 //       {
 //           // 슬롯 가시성 및 레이아웃 확인
 //           SharingUserSlot->SetVisibility(ESlateVisibility::Visible);
 //           SharingUserSlot->SetUserID(UserStreamID);
	//		SharingUserSlot->FUserIDButtonDelegate_OneParam.BindUFunction(this,FName("SetUserID"));
 //           // Grid에 슬롯 추가
 //           SharingUserPanel->AddChildToUniformGrid(SharingUserSlot, Row, Column);

 //           // Row 값 증가
 //           Row++;

 //           if (!SharingUserPanel)
 //           {
 //               UE_LOG(LogTemp, Error, TEXT("PartsPanel is not valid."));
 //               return;
 //           }

 //           //SharingUserSlot->clickcnt = P_clickcnt; // 클릭 값 전달 (계속 InvSlot 갱신돼서 clickcnt값 업데이트 안 되는 문제 때문)
 //       }
	//}
}

void ULSJMainWidget::LookStreaming(bool val)
{
	 bLookStreaming = val; 
	 if(false==val)
		ImageSharingScreen->SetVisibility(ESlateVisibility::Hidden);
}
