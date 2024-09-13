// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Recorderactor.generated.h"

UCLASS()
class METACHAT_API ARecorderactor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARecorderactor();
	//UserID //StreamID = Editor,Editor1,Editor2
	//스트리밍을 하지 않는다면 StreamID = "";
	TMap<FString,FString> StreamingUsers;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



};
