// Fill out your copyright notice in the Description page of Project Settings.


#include "HSB/CustomCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SB/CustomSaveGame.h"
#include "Engine/SkeletalMesh.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACustomCharacter::ACustomCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LowerBodyMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LowerBody"));
	LowerBodyMeshComp->SetupAttachment(GetMesh());
	UpperBodyMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("UpperBody"));
	UpperBodyMeshComp->SetupAttachment(GetMesh());
	HeadMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head"));
	HeadMeshComp->SetupAttachment(GetMesh());
	HairMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hair"));
	HairMeshComp->SetupAttachment(GetMesh());
	FeetMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Feet"));
	FeetMeshComp->SetupAttachment(GetMesh());
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
        UE_LOG(LogTemp, Warning, TEXT("%s"), **MeshPath_h);

        if (MeshPath_h)
        {
            USkeletalMesh* LoadedMesh = LoadObject<USkeletalMesh>(nullptr, **MeshPath_h);
            if (LoadedMesh)
            {
                HairMeshComp->SetSkeletalMesh(LoadedMesh);
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
                UpperBodyMeshComp->SetSkeletalMesh(LoadedMesh);
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
                LowerBodyMeshComp->SetSkeletalMesh(LoadedMesh);
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
                FeetMeshComp->SetSkeletalMesh(LoadedMesh);
                UE_LOG(LogTemp, Warning, TEXT("Feet mesh has been applied from save."));
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load save game"));
    }
}

void ACustomCharacter::ServerUpdateCustomizationData_Implementation(const FCharacterCustomizationData& NewData)
{
    // 서버 -> 다른 클라이언트 (변경된 데이터 전송)
    if (HasAuthority())
    {
        CustomizationData = NewData;
        MulticastUpdateCustomizationData(NewData);
    }
}

bool ACustomCharacter::ServerUpdateCustomizationData_Validate(const FCharacterCustomizationData& NewData)
{
    return true;
}

void ACustomCharacter::MulticastUpdateCustomizationData_Implementation(const FCharacterCustomizationData& NewData)
{
    // 모든 클라이언트에게 적용
    CustomizationData = NewData;
    // 캐릭터 외형 갱신
    UpdateCharacterAppearance();
}

void ACustomCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // CustomizationData 구조체를 복제 목록에 추가
    DOREPLIFETIME(ACustomCharacter, CustomizationData);
}

void ACustomCharacter::UpdateCharacterAppearance()
{
    if (CustomizationData.HairMesh)
    {
        HairMeshComp->SetSkeletalMesh(CustomizationData.HairMesh);
    }
    if (CustomizationData.UpperBodyMesh)
    {
        HairMeshComp->SetSkeletalMesh(CustomizationData.UpperBodyMesh);
    }
    if (CustomizationData.LowerBodyMesh)
    {
        HairMeshComp->SetSkeletalMesh(CustomizationData.LowerBodyMesh);
    }
    if (CustomizationData.FeetMesh)
    {
        HairMeshComp->SetSkeletalMesh(CustomizationData.FeetMesh);
    }
}
