// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/Recorderactor.h"


// Sets default values
ARecorderactor::ARecorderactor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	//구역 콜리전
	//구역 안에 있는 StreamID 모음
	//구역 안에 있는 UserID 모음
	//소리 들리는 범위? 
	//VOIP //???

}

// Called when the game starts or when spawned
void ARecorderactor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARecorderactor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARecorderactor::AddUser(FString pUserID, FString pStreamID)
{
	StreamingUsers.Add(pUserID,pStreamID);
}

