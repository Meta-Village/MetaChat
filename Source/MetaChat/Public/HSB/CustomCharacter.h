// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CustomCharacter.generated.h"

USTRUCT(BlueprintType)
struct FCharacterCustomizationData
{
    GENERATED_BODY()
public:
    // Hair
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite , Category = "Customization")
    USkeletalMesh* HairMesh;
     // Upper Body
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite , Category = "Customization")
    USkeletalMesh* UpperBodyMesh;
    // Lower Body
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Customization")
    USkeletalMesh* LowerBodyMesh;
     // Feet
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite , Category = "Customization")
    USkeletalMesh* FeetMesh;
};

UCLASS()
class METACHAT_API ACustomCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACustomCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Lower Body
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* LowerBodyMeshComp;


    // Upper Body
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite , Category = "Character Parts")
    USkeletalMeshComponent* UpperBodyMeshComp;

    // Head
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite , Category = "Character Parts")
    USkeletalMeshComponent* HeadMeshComp;

    // Hair
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite , Category = "Character Parts")
    USkeletalMeshComponent* HairMeshComp;

    // Feet
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite , Category = "Character Parts")
    USkeletalMeshComponent* FeetMeshComp;

    // level 넘어갈 때 로드될 정보
	UFUNCTION(BlueprintCallable)
	void Load();

    // RPC 내용----------------------------------------
public:
    UFUNCTION(Server, Reliable, WithValidation)
    void ServerUpdateCustomizationData(const FCharacterCustomizationData& NewData);
    void ServerUpdateCustomizationData_Implementation(const FCharacterCustomizationData& NewData);
    bool ServerUpdateCustomizationData_Validate(const FCharacterCustomizationData& NewData);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastUpdateCustomizationData(const FCharacterCustomizationData& NewData);
    void MulticastUpdateCustomizationData_Implementation(const FCharacterCustomizationData& NewData);

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

    // 캐릭터 커스터마이징 상태
    UPROPERTY(Replicated, BlueprintReadWrite, Category = "Customization")
    FCharacterCustomizationData CustomizationData;

    // 캐릭터 외형 갱신
    void UpdateCharacterAppearance();

    // 레벨 전환 버튼바인딩 -> PlayerController 에서 처리
};
