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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Audio Component 선언
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Audio")
	class UAudioCaptureComponent* AudioCaptureComponent;

	// 녹음 시작
	void StartRecord();

	// 녹음 끝
	void EndRecord();

	// 녹음파일 저장하기
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio")
	class USoundWave* SoundWave;
};
