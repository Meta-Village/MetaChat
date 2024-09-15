﻿// Fill out your copyright notice in the Description page of Project Settings.

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
	UFUNCTION(BlueprintCallable)
	FString GetSharingUsers(TArray<FString> Users);
	void SetViewSharingUserID(FString ID);
	UFUNCTION(BlueprintCallable)
	bool GetLookStreaming();
	void UpdateTexture();
	UTexture2D* CaptureScreenToTexture();
	UTexture2D* CapturedTexture;
	class UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class USceneComponent* sceneComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
    class UStaticMeshComponent* WindowScreenPlaneMesh;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> MainWidgetFactory;
	UPROPERTY(BlueprintReadWrite)
	class ULSJMainWidget* MainWidget;

	//블루프린트 함수 호출
	void StopLookSharingScreen();
	void BeginLookSharingScreen();

	void ChangeLookSharingScreen();
	//ViewSharingUserID
	UPROPERTY(BlueprintReadWrite)
	FString UserID;
	UPROPERTY(BlueprintReadWrite)
	FString UserStreamID = "";
	UPROPERTY(BlueprintReadWrite)
	FString ViewSharingUserStreamID;

	UPROPERTY(EditDefaultsOnly, Category = "Capture")
	class UTextureRenderTarget2D* RenderTarget;
	UPROPERTY(EditDefaultsOnly, Category = "Capture")
	class USceneCaptureComponent2D* SceneCapture;

	//구역 정보에 UserID와 StreamID 넣기
	void AddUserInfoToArea(AActor* AreaActor);
};
