// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PixelStreamingSignallingComponent.h"
#include "PixelStreamingActor.generated.h"

UCLASS()
class METACHAT_API APixelStreamingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APixelStreamingActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable, Category = "PixelStreaming")
	void SetSignallingServerUrl(class UPixelStreamingSignallingComponent* Component , const FString& URL);
	class UPixelStreamingSignallingComponent* PixelStreamingSignallingComp;
};
