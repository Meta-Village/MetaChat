// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HttpFwd.h"
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

UENUM(BlueprintType)
enum class ELocationState : uint8 
{
	IDLE,
	MOVE
};

UCLASS()
class METACHAT_API ACustomCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACustomCharacter();
    void SetUpLocation(ELocationState State);
    void UpdateState();
    void Idle();
    void Move();

    UPROPERTY()
	class UCustomAnimInstance* CustomAnimInstance;

    UPROPERTY(EditDefaultsOnly)
	ELocationState CurrentState;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UPROPERTY(EditDefaultsOnly)
    class USpringArmComponent* SpringArm;
    UPROPERTY(EditDefaultsOnly)
    class UCameraComponent* Camera;

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
// 
//     // RPC 내용----------------------------------------
// public:
//     // 커스텀 데이터를 Listen server에 전송
//     UFUNCTION(Server, Reliable, WithValidation) // 클라 -> 리슨서버
//     void ServerUpdateCustomizationData(const FCharacterCustomizationData& NewData);
//     void ServerUpdateCustomizationData_Implementation(const FCharacterCustomizationData& NewData);
//     bool ServerUpdateCustomizationData_Validate(const FCharacterCustomizationData& NewData);
// 
//     UFUNCTION(NetMulticast, Reliable) // 서버 -> 여러 클라
//     void MulticastUpdateCustomizationData(const FCharacterCustomizationData& NewData);
//     void MulticastUpdateCustomizationData_Implementation(const FCharacterCustomizationData& NewData);
// 
//      virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
// 
//      UFUNCTION()
//      void OnRep_CustomizationData();
// 
//     // 캐릭터 커스터마이징 상태
//     UPROPERTY(ReplicatedUsing=OnRep_CustomizationData)
//     FCharacterCustomizationData CustomizationData;
// 
//     // 캐릭터 외형 갱신
//     void UpdateCharacterAppearance();
// 
//     // 네트워크 상태로그 출력 할 함수
//     void PrintNetLog();
// 
//     UPROPERTY(BlueprintReadWrite,ReplicatedUsing=OnRep_ChangeMatColor)
//     int MatColor;
//     UFUNCTION(BlueprintCallable)
//     void OnRep_ChangeMatColor();

// 
private:
    int32 CurrentLocationInfo;
//     // 서버에 CurrentLocationInfo 보내기
//     void SendLocationInfoToServer(FDateTime entry, FDateTime exist, FName zoneName, FString userId, int32 CurrentLocationInfo);
// 
//     // 서버 응답 처리
//     void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
