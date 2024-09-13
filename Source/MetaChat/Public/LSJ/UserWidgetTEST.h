// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../../../Plugins/Media/PixelStreaming/Source/PixelStreamingServers/Private/SignallingServer.h"
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

	//방 세션 UI


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

	bool bIsTaskCancelled = false;
	class AActor* windowViewer;
	class UStaticMeshComponent* PlaneMesh;
	class UMaterialInstanceDynamic* DynamicMaterial;

	//pixel streaming
	//TSharedPtr<UE::PixelStreamingServers::IServer> SignallingServerInstance;
	//TSharedPtr<UE::PixelStreamingServers::IServer> GetSignallingServer() const { return SignallingServerInstance; }
	void AccessSignallingServer();
	void StartLevelOnlyPixelStreaming();
	//FCriticalSection CriticalSection;
	//	TFuture<void> AsyncTaskHandle;

	//월드제목과 월드 비밀번호 보내기
	UFUNCTION(BlueprintCallable)
	virtual void SendCreatingWorldInfo();
	virtual void RecvCreatingWorldInfo(FString result,int32 resultCode);
	UPROPERTY(BlueprintReadWrite)
	FString WorldName;
	UPROPERTY(BlueprintReadWrite)
	FString WorldPassword;

	//회원가입정보 http 통신
	UFUNCTION(BlueprintCallable)
	virtual void SendCreatingIDInfo();

	//서버에서 월드번호,비밀번호로 조회결과 가져오기 http 통신

	UFUNCTION(BlueprintCallable)
	void SendFindSessionInfo(FString SendWorldID);
	void RecvFindSessionInfo(FString result, int32 resultCode);

	//세션 생성하는 블루프린트 함수
	UFUNCTION(BlueprintImplementableEvent, Category = "MyCategory")
    void CreateSession(int WorldID);
	//세션 참가하는 블루프린트 함수
	UFUNCTION(BlueprintImplementableEvent, Category = "MyCategory")
    void JoinSession(int WorldID);
};
