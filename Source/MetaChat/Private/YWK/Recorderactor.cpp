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
	SetReplicates(true);
}

// Called every frame
void ARecorderactor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARecorderactor::AddUser(FString pUserID, FString pStreamID)
{
	//ServerAddUserInfoToRecordActor(pUserID,pStreamID);
	FUserStreamingInfo Info;
	Info.UserID = pUserID;
	Info.UserStreamID = pStreamID;
	UserStreamingInfo.Add(Info);

	for (FUserStreamingInfo InfoID : UserStreamingInfo)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Black, FString::Printf(TEXT("FUserStreamingInfo : ID : %s  / stream : %s"),*(InfoID.UserID),*(InfoID.UserStreamID)));
	}
}
void ARecorderactor::RemoveUser(FString pUserID)
{
	for (int i = 0; i < UserStreamingInfo.Num(); i++)
	{
		if (UserStreamingInfo[i].UserID.Equals(pUserID))
		{
			UserStreamingInfo.RemoveAt(i);
			break;
		}
	}
}

void ARecorderactor::ServerAddUserInfoToRecordActor_Implementation(const FString& pUserID,const FString& pStreamUserID)
{
	FUserStreamingInfo Info;
	Info.UserID = pUserID;
	Info.UserStreamID = pStreamUserID;
	UserStreamingInfo.Add(Info);

	for (FUserStreamingInfo InfoID : UserStreamingInfo)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Black, FString::Printf(TEXT("FUserStreamingInfo : ID : %s  / stream : %s"),*(InfoID.UserID),*(InfoID.UserStreamID)));
	}
}
