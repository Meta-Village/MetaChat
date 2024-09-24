// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HAL/CriticalSection.h"
#include "ScreenActor.generated.h"

UCLASS()
class METACHAT_API AScreenActor : public AActor
{
	GENERATED_BODY()
	FCriticalSection CriticalSection;

public:	
	// Sets default values for this actor's properties
	AScreenActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	//화면 공유 시작시 StreamID를 지정한다.
	UFUNCTION(BlueprintCallable)
	void UserStartStreaming(TArray<FString> Users);
	//StreamID 목록을 가져온다.
	UFUNCTION(BlueprintCallable)
	FString GetSharingUsers(TArray<FString> Users);
	//공유된 화면보기 - ID에 해당하는 Streaming화면을 본다.
	void SetViewSharingUserID(FString ID);
	//공유된 화면을 시청 중 true / false
	UFUNCTION(BlueprintCallable)
	bool GetLookStreaming();
	//일정 시간 간격으로 화면 캡처 수행
	//PostProcessDynamicMaterial 에 모니터화면 이미지를 넣는다.
	void UpdateTexture();
	//모니터화면을 UTexture2D로 만든다.
	UTexture2D* CaptureScreenToTexture();
	//화면 공유를 중단한다.
	UFUNCTION(BlueprintCallable)
	void StopLookSharingScreenWidget();
	//모니터화면을 UTexture2D로 담는 변수
	UTexture2D* CapturedTexture;
	//WindowScreenPlaneMesh의 Material - 모니터화면을 보여줄 매터리얼
	class UMaterialInstanceDynamic* DynamicMaterial;
	//RootComponent
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class USceneComponent* sceneComp;
	//Plane - 모니터화면을 보여줄 Plane
	UPROPERTY(EditDefaultsOnly, Category = "Components")
    class UStaticMeshComponent* WindowScreenPlaneMesh;
	//MainWidget - 화면공유 버튼과 공유자목록 버튼, 화면공유창
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> MainWidgetFactory;
	UPROPERTY(BlueprintReadWrite)
	class ULSJMainWidget* MainWidget;

	//블루프린트 함수 호출
	//공유된 화면 그만보기
	void StopLookSharingScreen();
	//블루프린트 함수 호출
	//공유된 화면 보기
	void BeginLookSharingScreen();
	//블루프린트 함수 호출
	//공유된 화면 다른 StreamID로 교체
	void ChangeLookSharingScreen();
	//유저 아이디
	UPROPERTY(BlueprintReadWrite)
	FString UserID;
	//유저의 StreamID
	UPROPERTY(BlueprintReadWrite)
	FString UserStreamID = "";
	//유저가 보는 화면 공유자의 StreamID
	UPROPERTY(BlueprintReadWrite)
	FString ViewSharingUserStreamID;

	//PixelStreaming 에 보여줄 Texture를 담을 변수
	UPROPERTY(EditDefaultsOnly, Category = "Capture")
	TObjectPtr <class UTextureRenderTarget2D> RenderTarget;
	//카메라에 보이는 것을 캡쳐하기 위한 변수
	UPROPERTY(EditDefaultsOnly, Category = "Capture")
	class USceneCaptureComponent2D* SceneCapture;

	//포스트 프로세스에 모니터화면이 보이게 하는 것
    UPROPERTY()
    class APostProcessVolume* PostProcessVolume;
	class UMaterialInstanceDynamic* PostProcessDynamicMaterial;
};
