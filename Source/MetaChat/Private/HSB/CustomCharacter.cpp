// Fill out your copyright notice in the Description page of Project Settings.


#include "HSB/CustomCharacter.h"
#include "../MetaChat.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SB/CustomSaveGame.h"
#include "Engine/SkeletalMesh.h"
#include "Net/UnrealNetwork.h"
#include "../MetaChatPlayerController.h"

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
	
    bReplicates = true;
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
                // SavedMeshes에서 데이터를 가져와 FCharacterCustomizationData 구조체로 변환
                CustomizationData.HairMesh = LoadedMesh;
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
                // SavedMeshes에서 데이터를 가져와 FCharacterCustomizationData 구조체로 변환
                CustomizationData.UpperBodyMesh = LoadedMesh;
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
                // SavedMeshes에서 데이터를 가져와 FCharacterCustomizationData 구조체로 변환
                CustomizationData.LowerBodyMesh = LoadedMesh;
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
                // SavedMeshes에서 데이터를 가져와 FCharacterCustomizationData 구조체로 변환
                CustomizationData.FeetMesh = LoadedMesh;
                 FeetMeshComp->SetSkeletalMesh(LoadedMesh);
                UE_LOG(LogTemp, Warning, TEXT("Feet mesh has been applied from save."));
            }
        }

        
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load save game"));
    }

    // SubmitCustomizationData 함수 호출
    
    if (IsLocallyControlled())
    {
//         auto* pc = Cast<AMetaChatPlayerController>(GetController());
//         pc->SubmitcustomizationData(CustomizationData);
        ServerUpdateCustomizationData(CustomizationData);
    }
    UpdateCharacterAppearance();
}

// 클라 -> 리슨서버
void ACustomCharacter::ServerUpdateCustomizationData_Implementation(const FCharacterCustomizationData& NewData)
{
/*    if (IsLocallyControlled())*/
    {
        CustomizationData = NewData;
        //MulticastUpdateCustomizationData(NewData);
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

void ACustomCharacter::OnRep_CustomizationData()
{
    // 복제된 커스터마이제이션 데이터를 기반으로 캐릭터 외형을 업데이트
    UpdateCharacterAppearance();
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

void ACustomCharacter::PrintNetLog()
{
    const FString conStr = GetNetConnection() != nullptr ? TEXT("Valid Connection") : TEXT("InValid Connection");

    const FString ownerName = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");

    const FString logStr = FString::Printf(TEXT("Connection : %s\nOwner Name : %s\nLocalRole : %s\nRemoteRole : %s"), *conStr, *ownerName, *LOCALROLE, *REMOTEROLE);

/*    DrawDebugString(GetWorld(), GetActorLocation() + FVector::UpVector * 100, logStr, nullptr, FColor::Red, 0, true, 1);*/
}

void ACustomCharacter::OnRep_ChangeMatColor()
{
/*    MatColor = FMath::RandRange(0, 100);*/
    const FString logStr = FString::Printf(TEXT("Connection: %d"), MatColor);
    DrawDebugString(GetWorld(), GetActorLocation() + FVector::UpVector * 100, logStr, nullptr, FColor::Red, 0, true, 1);
}
