﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidgetTEST.generated.h"

class UTexture2D;
/**
 * 
 */
UCLASS()
class METACHAT_API UUserWidgetTEST : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* ButtonTest;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* ButtonJoin;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* HttpGetButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* HttpPostButton;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextLog;
	 UPROPERTY(meta = (BindWidget))
    class UImage* ImageWidget; 
protected:
	virtual void NativeOnInitialized();

	virtual void NativeConstruct() override;
	virtual void NativeDestruct();
private:
	// 버튼을 눌렀을 때, 호출될 델리게이트에 등록할 함수
	UFUNCTION(BlueprintCallable)
	void TestButtonCallback();
	UFUNCTION(BlueprintCallable)
	void JoinCallback();
	UFUNCTION(BlueprintCallable)
	void OnMyClickSendPost();

	UFUNCTION(BlueprintCallable)
	void OnMyClickSendGet();

	class AHttpActor* HttpActor;
public:
	void SetHttpActor(class AHttpActor* actor);
	void SetTextLog(FString log);

	//window api viewer
	void SetImageTexture(class UTexture2D* Texture);
	UTexture2D* GetImageTexture();
	void SetViewer(AActor* actor);
	void UpdateWidgetTexture();
	UTexture2D* CaptureScreenToTexture();
	UTexture2D* CapturedTexture;
	TFuture<void> AsyncTaskHandle;
	bool bIsTaskCancelled = false;
	class AActor* windowViewer;
	class UStaticMeshComponent* PlaneMesh;
		class UMaterialInstanceDynamic * DynamicMaterial;
};
