// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VoiceComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METACHAT_API UVoiceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVoiceComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// VOIP Talker 컴포넌트 선언부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Voice Chat", meta = (AllowPrivateAccess = "true"))
	class UVOIPTalker* VOIPTalkerComponent;

	// 마이크 임계값 설정
	void SetMicThreshold(float Threshold);

	// 플레이어 상태 등록
	void RegisterWithPlayerState();

	// 로컬 플레이어가 제어중인지 확인
	bool IsLocallyControlled() const;

	// VOIP 관련 초기화 작업
	void InitiallizeVOIP();

	// 음성 통신 활성/비활성 시킬거
	void ToggleSpeaking(bool bIsSpeaking);

	// 입력 컴퍼넌트 설정 함수
	void SetupInputComponent();

	// 음성 통신 시작
	void StartSpeaking();

	// 음성 통신 끝
	void EndSpeaking();

private:
	// 캐시된 소유자 참조
	UPROPERTY()
	AActor* CachedOwner;

	// 캐시된 Pawn 참조
	UPROPERTY()
	APawn* CachedPawn;

	// 캐시된 PlayerController 참조
	UPROPERTY()
	APlayerController* CachedPlayerController;

	// 사운드 아이콘 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> SoundWidgetClass;

	// 현재 화면에 표시된 사운드 아이콘 위젯 참조
	UPROPERTY()
	class UUserWidget* SoundWidget;
		
};
