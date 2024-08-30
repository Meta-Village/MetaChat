// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpActor.generated.h"

UCLASS()
class METACHAT_API AHttpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHttpActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> HttpUIFactory;
	UPROPERTY()
	class UUserWidgetTEST* HttpUI;

	// Send ��ư�� �������� ��û�� �Լ�
	void RsqGetTest(FString url);
	//������� �Լ�

	void OnRsqGetTest(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	void RsqPostTest(FString url, FString json);
	void OnResPostTest(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

};