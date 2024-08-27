// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidgetTEST.generated.h"

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

protected:
	virtual void NativeOnInitialized();
    
private:
	// 버튼을 눌렀을 때, 호출될 델리게이트에 등록할 함수
	UFUNCTION(BlueprintCallable)
	void TestButtonCallback();
	UFUNCTION(BlueprintCallable)
	void JoinCallback();
};
