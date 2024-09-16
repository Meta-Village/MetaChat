// Fill out your copyright notice in the Description page of Project Settings.


#include "HSB/ChairActor.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AChairActor::AChairActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("boxComp"));
	SetRootComponent(boxComp);
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(RootComponent);

	arrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("arrowComp"));
	arrowComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AChairActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChairActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

