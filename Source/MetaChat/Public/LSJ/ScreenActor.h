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
	UFUNCTION(BlueprintCallable)
	FString GetSharingUsers(TArray<FString> Users);
	void SetViewSharingUserID(FString ID);
	void UpdateTexture();
	UTexture2D* CaptureScreenToTexture();
	UTexture2D* CapturedTexture;
	class UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	class USceneComponent* sceneComp;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Components")
    class UStaticMeshComponent* WindowScreenPlaneMesh;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> MainWidgetFactory;
	UPROPERTY(BlueprintReadWrite)
	class ULSJMainWidget* MainWidget;

	//블루프린트 함수 호출
	void StopLookSharingScreen();
	void BeginStreaming();
	void BeginLookSharingScreen();

	void ChangeLookSharingScreen();
	//ViewSharingUserID
	UPROPERTY(BlueprintReadWrite)
	FString UserID;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Capture")
    class UTextureRenderTarget2D* RenderTarget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Capture")
	class USceneCaptureComponent2D* SceneCapture;
};
