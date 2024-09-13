// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChairActor.generated.h"

UCLASS()
class METACHAT_API AChairActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChairActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* boxComp;
	UPROPERTY(EditDefaultsOnly)
	class UMeshComponent* meshComp;

	UPROPERTY(EditDefaultsOnly)
	class UArrowComponent* arrowComp;
};
