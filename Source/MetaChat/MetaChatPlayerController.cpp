// Copyright Epic Games, Inc. All Rights Reserved.

#include "MetaChatPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "MetaChatCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "HSB/CustomCharacter.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputAction.h"
#include "HSB/CustomAnimInstance.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AMetaChatPlayerController::AMetaChatPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void AMetaChatPlayerController::AddChats(TArray<FString>& ArrayUserID, TArray<FString>& ArrayChatContent)
{
	// 블루프린트 함수 이름
	FName FunctionName(TEXT("AddChats")); // 블루프린트에서 정의한 함수명

	// 블루프린트 함수 가져오기
	UFunction* Function = FindFunction(FunctionName);

	if (Function)
	{
		struct FMyFunctionParams
		{
			TArray<FString> ParamsArrayUserID;
			TArray<FString> ParamsArrayChatContent;
		};

		// 구조체에 파라미터 값 세팅
		FMyFunctionParams Params;
		Params.ParamsArrayUserID = ArrayUserID;         // int32
		Params.ParamsArrayChatContent = ArrayChatContent;      // float

		// ProcessEvent로 블루프린트 함수 호출, 파라미터로 구조체 전달
		ProcessEvent(Function, &Params);
		// 블루프린트 함수 호출 (매개변수가 없는 경우)
		//ProcessEvent(Function, ArrayUserID,ArrayChatContent);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Function not found: %s"), *FunctionName.ToString());
	}
}

void AMetaChatPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void AMetaChatPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AMetaChatPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AMetaChatPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AMetaChatPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AMetaChatPlayerController::OnSetDestinationReleased);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &AMetaChatPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &AMetaChatPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &AMetaChatPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &AMetaChatPlayerController::OnTouchReleased);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMetaChatPlayerController::OnInputStarted()
{
	StopMovement();
	OnSetDestinationTriggered();  // 입력이 시작될 때 바로 이동을 시도
}

// Triggered every frame when the input is held down
void AMetaChatPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AMetaChatPlayerController::OnSetDestinationReleased()
{
// 	// If it was a short press
// 	if (FollowTime <= ShortPressThreshold)
// 	{
// 		// We move there and spawn some particles
// 		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
// 		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
// 	}

	// 서버에서 이동 처리
	if (HasAuthority())
	{
		MoveToLocation(CachedDestination);

	}
	else
	{
		ServerMoveToLocation_Implementation(CachedDestination);
	}
	
	// 이동클릭 이펙트
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);

	FollowTime = 0.f;
}

void AMetaChatPlayerController::MoveToLocation(const FVector& Location)
{
	CachedDestination = Location;
	// 서버에서 지속적으로 이동 처리
	GetWorld()->GetTimerManager().SetTimer(MoveTimerHandle, this, &AMetaChatPlayerController::MoveToLocationTick, GetWorld()->GetDeltaSeconds(), true);
}

void AMetaChatPlayerController::MoveToLocationTick()
{
	ACustomCharacter* Customcharacter = Cast<ACustomCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	if (nullptr == Customcharacter)
		return;

	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		// 캐릭터의 속도 확인
		FVector Velocity = ControlledPawn->GetVelocity();
		float Speed = Velocity.Size();  // 속도의 크기를 계산

		// 목표 지점까지의 거리 계산
		float DistanceSquared = FVector::DistSquared(ControlledPawn->GetActorLocation(), CachedDestination);
		const float StopDistanceSquared = FMath::Square(50.0f);  // 허용 오차 범위

		if (Customcharacter->GetMesh())
		{
			CustomAnimInstance = Cast<UCustomAnimInstance>(Customcharacter->GetMesh()->GetAnimInstance());
		}

		// 속도가 0이 되고, 의자에 앉지 않을 때 IDLE 상태로 전환
		if (Speed <= 1.0f && CustomAnimInstance->IsSitting == false)
		{
			Customcharacter->SetUpLocation(ELocationState::IDLE);
// 			UE_LOG(LogTemp, Warning, TEXT("Character set to IDLE state"));
		}
		// 아래 도달 부분은 실행이 안 됨.
		if (DistanceSquared <= StopDistanceSquared)
		{
			// 도착 지점 근처에 도달한 경우 타이머를 멈추고 이동 종료
			GetWorld()->GetTimerManager().ClearTimer(MoveTimerHandle);

			UE_LOG(LogTemp, Warning, TEXT("Arrived at Destination"));
		}
		else
		{
			// 목표 지점에 도달하지 않았을 때만 이동 입력을 추가
			FVector Direction = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(Direction, 1.0f, false);
			if(Speed > 1.0f && CustomAnimInstance->IsSitting == false)
			{
				// MOVE 상태로 전환 (의자에 다가가지 않았을 때만)
				Customcharacter->SetUpLocation(ELocationState::MOVE);
			}
		}
	}
}

void AMetaChatPlayerController::ServerMoveToLocation_Implementation(const FVector& Location)
{
	// 서버에서 이동 처리
	MoveToLocation(Location);
}

bool AMetaChatPlayerController::ServerMoveToLocation_Validate(const FVector& Location)
{
	return true;
}



// Triggered every frame when the input is held down
void AMetaChatPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void AMetaChatPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}
