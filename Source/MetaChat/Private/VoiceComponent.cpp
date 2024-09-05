// Fill out your copyright notice in the Description page of Project Settings.


#include "VoiceComponent.h"
#include "GameFramework/PlayerState.h"
#include "Components/AudioComponent.h"
#include "TimerManager.h"
#include "Net/VoiceConfig.h"
#include "Blueprint/UserWidget.h"

// Sets default values for this component's properties
UVoiceComponent::UVoiceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
	// VOIP Talker 컴포넌트를 생성하고, VOIPTalkerComponent 포인터에 할당합니다.
	VOIPTalkerComponent = CreateDefaultSubobject<UVOIPTalker>(TEXT("VOIPTalker"));
}


// Called when the game starts
void UVoiceComponent::BeginPlay()
{
	Super::BeginPlay();

	// VOIP 초기화 작업 호출 
	InitiallizeVOIP();

	// Player State 가져오기
	AActor* Owner = GetOwner();
	if (Owner)
	{
		APawn* Pawn = Cast<APawn>(Owner);
		if (Pawn && Pawn->GetPlayerState())
		{
			APlayerState* PlayerState = Pawn->GetPlayerState();
		}
	}

}


// Called every frame
void UVoiceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UVoiceComponent::SetMicThreshold(float Threshold)
{
	if (VOIPTalkerComponent)
	{
		UVOIPStatics::SetMicThreshold(Threshold);
	}
}

void UVoiceComponent::RegisterWithPlayerState()
{
	AActor* Owner = GetOwner();
	if (VOIPTalkerComponent && Owner)
	{
		APawn* Pawn = Cast<APawn>(Owner);
		if (Pawn)
		{
			APlayerState* PlayerState = Pawn->GetPlayerState();
			if (PlayerState)
			{
				// VOIPTalkerComponent를 PlayerState와 연동
				VOIPTalkerComponent->RegisterWithPlayerState(PlayerState);
			}
		}
	}
}

bool UVoiceComponent::IsLocallyControlled() const
{
	AActor* Owner = GetOwner();
	if (Owner)
	{
		APawn* Pawn = Cast<APawn>(Owner);
		if (Pawn)
		{
			return Pawn->IsPlayerControlled();
		}
	}
	return false;
}

void UVoiceComponent::InitiallizeVOIP()
{
	if (IsValid(VOIPTalkerComponent))
	{
		// VOIPTalker가 유효한지 검사하기
		if (IsValid(VOIPTalkerComponent))
		{
			// 플레이어 상태에 VOIPTalker를 등록하기
			RegisterWithPlayerState();

			// 마이크 임계값 설정
			SetMicThreshold(-1.0f);

			// 로컬 플레이어가 제어 중일 때만 VOIP 관련 설정 진행
			if (IsLocallyControlled())
			{
				AActor* Owner = GetOwner();
				if (Owner)
				{
					APawn* Pawn = Cast<APawn>(Owner);
					if (Pawn)
					{
						// 콘솔 명령 실행하여 VOIP 활성화 하기
						APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
						if (PlayerController)
						{
							PlayerController->ConsoleCommand("OSS.VoiceLoopback 1");
						}
					}
				}
			}
		}
	}

}

void UVoiceComponent::ToggleSpeaking(bool bIsSpeaking)
{
	if (bIsSpeaking)
	{
		// VOIP 활성화
		if (IsLocallyControlled() && CachedPlayerController)
		{
			CachedPlayerController->ConsoleCommand("ToggleSpeaking 1");

			// 사운드 아이콘 위젯 생성 및 화면 추가
			if (SoundWidget)
			{
				SoundWidget = CreateWidget<UUserWidget>(CachedPlayerController, SoundWidgetClass);
				if (SoundWidget)
				{
					SoundWidget->AddToViewport();
				}
			}
		}
	}
	else
	{
		// VOIP 비활성화
		if (IsLocallyControlled() && CachedPlayerController)
		{
			CachedPlayerController->ConsoleCommand("ToggleSpeaking 0");
			if (SoundWidget)
			{
				SoundWidget->RemoveFromParent();
				SoundWidget = nullptr;
			}
		}
	}

}

void UVoiceComponent::SetupInputComponent()
{
	if (CachedPawn && CachedPawn->InputComponent)
	{
		CachedPawn->InputComponent->BindAction("ToggleSpeaking", IE_Pressed, this, &UVoiceComponent::StartSpeaking);
		CachedPawn->InputComponent->BindAction("ToggleSpeaking", IE_Released, this, &UVoiceComponent::EndSpeaking);
	}
}
void UVoiceComponent::StartSpeaking()
{
	ToggleSpeaking(true);
}

void UVoiceComponent::EndSpeaking()
{
	ToggleSpeaking(false);
}
