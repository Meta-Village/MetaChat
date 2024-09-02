// Fill out your copyright notice in the Description page of Project Settings.


#include "HSB/CustomCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SB/CustomSaveGame.h"

// Sets default values
ACustomCharacter::ACustomCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LowerBodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LowerBody"));
	LowerBodyMesh->SetupAttachment(GetMesh());
	UpperBodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("UpperBody"));
	UpperBodyMesh->SetupAttachment(GetMesh());
	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head"));
	HeadMesh->SetupAttachment(GetMesh());
	HairMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hair"));
	HairMesh->SetupAttachment(GetMesh());
	FeetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Feet"));
	FeetMesh->SetupAttachment(GetMesh());
}

// Called when the game starts or when spawned
void ACustomCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACustomCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACustomCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACustomCharacter::Load()
{
    // 저장된 데이터를 로드하고 캐릭터에 반영
    UCustomSaveGame* LoadedGameInstance = Cast<UCustomSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("CharacterMeshSaveSlot"), 0));
    if (LoadedGameInstance)
    {
        // HairMesh 로드 및 적용
        FString* MeshPath_h = LoadedGameInstance->SavedMeshes.Find("Hair");
        if (MeshPath_h)
        {
            USkeletalMesh* LoadedMesh = LoadObject<USkeletalMesh>(nullptr, **MeshPath_h);
            if (LoadedMesh)
            {
                HairMesh->SetSkeletalMesh(LoadedMesh);
                UE_LOG(LogTemp, Warning, TEXT("Hair mesh has been applied from save."));
            }
        }

        // UpperBodyMesh 로드 및 적용
        FString* MeshPath_u = LoadedGameInstance->SavedMeshes.Find("Upper");
        if (MeshPath_u)
        {
            USkeletalMesh* LoadedMesh = LoadObject<USkeletalMesh>(nullptr, **MeshPath_u);
            if (LoadedMesh)
            {
                UpperBodyMesh->SetSkeletalMesh(LoadedMesh);
                UE_LOG(LogTemp, Warning, TEXT("Upper body mesh has been applied from save."));
            }
        }

        // LowerBodyMesh 로드 및 적용
        FString* MeshPath_l = LoadedGameInstance->SavedMeshes.Find("Lower");
        if (MeshPath_l)
        {
            USkeletalMesh* LoadedMesh = LoadObject<USkeletalMesh>(nullptr, **MeshPath_l);
            if (LoadedMesh)
            {
                LowerBodyMesh->SetSkeletalMesh(LoadedMesh);
                UE_LOG(LogTemp, Warning, TEXT("Lower body mesh has been applied from save."));
            }
        }

        // FeetMesh 로드 및 적용
        FString* MeshPath_f = LoadedGameInstance->SavedMeshes.Find("Feet");
        if (MeshPath_f)
        {
            USkeletalMesh* LoadedMesh = LoadObject<USkeletalMesh>(nullptr, **MeshPath_f);
            if (LoadedMesh)
            {
                FeetMesh->SetSkeletalMesh(LoadedMesh);
                UE_LOG(LogTemp, Warning, TEXT("Feet mesh has been applied from save."));
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load save game"));
    }
}