// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Templates/SharedPointer.h" 
#include "MeetingButton.generated.h"

/**
 * 
 */
UCLASS()
class METACHAT_API UMeetingButton : public UUserWidget
{
	GENERATED_BODY()

public:
	
	int32 meetingId = 0;

	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* Bt_MeetingStart;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* Bt_MeetingEnd;

	UFUNCTION(BlueprintCallable)
	void MeetingStart_Clicked();

	UFUNCTION()
	void MeetingEnd_Clicked();

	UFUNCTION(BlueprintCallable, Category = "Meeting")
	void OnMeetingStart(int32 meetingId, const FString& meetStartTime, const FString& meetEndTime, const FString& ZoneName, int32 WorldId);

	UFUNCTION()
	void SendMeetingStart(const FString& JsonString);

	UFUNCTION()
	void SendMeetingEnd(int32 meetingId, const FString& meetEndTime);

	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
