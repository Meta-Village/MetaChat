// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IPixelStreamingModule.h" // PixelStreamingModule 헤더 포함
#include "PixelStreamingActor.generated.h"


class UTexture2D;

UCLASS()
class METACHAT_API APixelStreamingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APixelStreamingActor();

    // 스트리밍 시작
    UFUNCTION(BlueprintCallable, Category = "PixelStreaming")
    void StartPixelStreaming();

    // 스트리밍 종료
    UFUNCTION(BlueprintCallable, Category = "PixelStreaming")
    void StopPixelStreaming();

    //window viewer
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* PlaneMesh;
    class UMaterialInstanceDynamic* DynamicMaterial;
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Tick( float DeltaSeconds );
    void StartServers();

    //window viewer
    void UpdateWidgetTexture();
    UTexture2D* CaptureScreenToTexture();
    TFuture<void> AsyncTaskHandle;
    bool bIsTaskCancelled = false;
    class AActor* windowViewer;
private:
    // 스트리밍을 관리하는 스트리머 포인터
    TSharedPtr<IPixelStreamingStreamer> MyStreamer;
};