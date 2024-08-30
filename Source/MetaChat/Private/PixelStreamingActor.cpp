// Fill out your copyright notice in the Description page of Project Settings.


#include "PixelStreamingActor.h"

// Sets default values
APixelStreamingActor::APixelStreamingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    PixelStreamingSignallingComp = CreateDefaultSubobject<UPixelStreamingSignallingComponent>(TEXT("PixelStreamingSignallingComp"));
}
void APixelStreamingActor::SetSignallingServerUrl(UPixelStreamingSignallingComponent* Component , const FString& URL)
{
    if ( PixelStreamingSignallingComp )
    {
       // PixelStreamingSignallingComp->SetSignallingServerUrl(TEXT("master-of-prediction.shop:8890"));
    }
}
// Called when the game starts or when spawned
void APixelStreamingActor::BeginPlay()
{
	Super::BeginPlay();
    if ( PixelStreamingSignallingComp )
    {
        FString url = "";
        SetSignallingServerUrl(PixelStreamingSignallingComp, url);
        // 신호 서버에 연결하고 스트리밍 시작
       // PixelStreamingSignallingComp->Subscribe();
    }

}

// Called every frame
void APixelStreamingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

