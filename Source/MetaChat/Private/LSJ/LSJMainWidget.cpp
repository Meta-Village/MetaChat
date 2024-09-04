
// Fill out your copyright notice in the Description page of Project Settings.


#include "LSJ/LSJMainWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "LSJ/ScreenActor.h"
#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreaming/Public/IPixelStreamingStreamer.h"
#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreaming/Public/PixelStreamingVideoInputBackBuffer.h"
#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreaming/Public/IPixelStreamingModule.h"
#include "Modules/ModuleManager.h"
#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreamingEditor/Public/PixelStreamingVideoInputViewport.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSubsystem.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/OnlineSessionSettings.h"

#include "LevelEditorViewport.h"
#include "Slate/SceneViewport.h"
#include "EditorViewportClient.h"


void ULSJMainWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}
void ULSJMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry,InDeltaTime);
	if (bStreaming && nullptr != ScreenActor)
	{
		ScreenActor->UpdateTexture();
	}
}
void ULSJMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ButtonLookSharingScreen->OnClicked.AddDynamic(this,&ULSJMainWidget::OnButtonLookSharingScreen);
	ButtonWindowScreen->OnClicked.AddDynamic(this, &ULSJMainWidget::OnButtonWindowScreen);
	ImageSharingScreen->SetVisibility(ESlateVisibility::Hidden);
}

void ULSJMainWidget::OnButtonWindowScreen()
{
	bStreaming = !bStreaming;
	FString streamID = "Editor";
	if (bStreaming)
	{
		ScreenActor->WindowScreenPlaneMesh->SetVisibility(true);
		//ScreenActor->BeginStreaming();
		// 1. PixelStreaming 모듈을 가져옵니다.
		IPixelStreamingModule* PixelStreamingModule = FModuleManager::GetModulePtr<IPixelStreamingModule>("PixelStreaming");
		
		if (PixelStreamingModule)
		{
			// 현재 세션의 아이디를 가져와서 Streamer를 생성한다.
			TSharedPtr<IPixelStreamingStreamer> Streamer = PixelStreamingModule->FindStreamer(streamID);//GetCurrentSessionID());
			//TSharedPtr<IPixelStreamingStreamer> Streamer = PixelStreamingModule->CreateStreamer("Test01");
			if (Streamer.IsValid())
			{
				/*FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
				TSharedPtr<SLevelViewport> ActiveLevelViewport = LevelEditorModule.GetFirstActiveLevelViewport();
				if (!ActiveLevelViewport.IsValid())
				{
					return;
				}

				FLevelEditorViewportClient& LevelViewportClient = ActiveLevelViewport->GetLevelViewportClient();
				FSceneViewport* SceneViewport = static_cast<FSceneViewport*>(LevelViewportClient.Viewport);
				Streamer->SetTargetViewport(SceneViewport->GetViewportWidget());
				Streamer->SetTargetWindow(SceneViewport->FindWindow());
				Streamer->SetInputHandlerType(EPixelStreamingInputType::RouteToWindow);
				Streamer->SetVideoInput(FPixelStreamingVideoInputViewport::Create(Streamer));*/
				{
					TSharedPtr<FPixelStreamingVideoInputBackBuffer> VideoInput = FPixelStreamingVideoInputBackBuffer::Create();
					//Back Buffer를 비디오 입력으로 설정합니다.
					Streamer->SetInputHandlerType(EPixelStreamingInputType::RouteToWidget);
					Streamer->SetVideoInput(FPixelStreamingVideoInputViewport::Create(Streamer));
					Streamer->SetSignallingServerURL("ws://master-of-prediction.shop:8890");
					
					//스트리밍을 시작합니다.
					Streamer->StartStreaming();
				}
				
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
		ScreenActor->WindowScreenPlaneMesh->SetVisibility(false);

		// 1. PixelStreaming 모듈을 가져옵니다.
		IPixelStreamingModule* PixelStreamingModule = FModuleManager::GetModulePtr<IPixelStreamingModule>("PixelStreaming");

		if (PixelStreamingModule)
		{
			// 2. 스트리머를 가져옵니다.
			TSharedPtr<IPixelStreamingStreamer> Streamer = PixelStreamingModule->FindStreamer(streamID);

			if (Streamer.IsValid())
			{
				// 4. 스트리밍을 시작합니다.
				Streamer->StopStreaming();
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
}

void ULSJMainWidget::OnButtonLookSharingScreen()
{
	bLookStreaming = !bLookStreaming;
	if (bLookStreaming)
	{
		ImageSharingScreen->SetVisibility(ESlateVisibility::Visible);
		//블루프린트 subs
		ScreenActor->BeginLookSharingScreen();
	}
	else
	{
		ImageSharingScreen->SetVisibility(ESlateVisibility::Hidden);
		//블루프린트 subs
		ScreenActor->StopLookSharingScreen();
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