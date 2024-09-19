// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Recorderactor.generated.h"

USTRUCT(Atomic,BlueprintType)
struct FUserStreamingInfo
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FString UserID;
	UPROPERTY(EditAnywhere)
	FString UserStreamID;
	UPROPERTY(EditAnywhere)
	bool bClicked = false;
};
UCLASS()
class METACHAT_API ARecorderactor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARecorderactor();
	//UserID //StreamID = Editor,Editor1,Editor2
	//스트리밍을 하지 않는다면 StreamID = "";
	//UPROPERTY(VisibleAnywhere,Replicated)
	UPROPERTY(VisibleAnywhere,ReplicatedUsing = OnRep_UpdateSlot)
	TArray<FUserStreamingInfo> UserStreamingInfo;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void AddUser(FString pUserID, FString pStreamID);


	void RemoveUser(FString pUserID);

	void UpdateUser(FString pUserID, FString pStreamID);
	UFUNCTION(Server, Reliable)
	void ServerAddUserInfoToRecordActor(const FString& pUserID,const FString& pStreamUserID);
	UFUNCTION()
	void OnRep_UpdateSlot();
};
