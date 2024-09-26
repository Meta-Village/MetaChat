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
    // TSoftObjectPtr로 선언하면 객체가 사용될 때 로드될 수 있음
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
	MOVE,
    SIT
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
    void Sit();
    void SitIdle();

    FTimerHandle handle;

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

    // CustomWidget의 P_clickcnt가 초기화 되므로 계속 로딩해줄 변수
	UPROPERTY(EditDefaultsOnly)
	int32 P_ClickLoad;

    // level 넘어갈 때 로드될 정보
	UFUNCTION(BlueprintCallable)
	void Load();

    // RPC 내용----------------------------------------
public:
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

    UFUNCTION(Server, Reliable)
    void ServerSetSkeletalMesh(USkeletalMesh* NewMesh, FName MeshCategory);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastUpdateSkeletalMesh(USkeletalMesh* NewMesh, FName MeshCategory);

    // 캐릭터 커스터마이징 상태
    UPROPERTY(ReplicatedUsing = OnRep_CustomizationData)
    FCharacterCustomizationData CustomizationData;

    UFUNCTION()
    void OnRep_CustomizationData();

    void UpdateCharacterAppearance();

    // 서버에 보낼 데이터들
    FDateTime EntryTime;
    FDateTime ExitTime;
    UPROPERTY(BlueprintReadWrite)
    FString ZoneName;
    FString UserId;
    int32 WorldId;
    
    // 서버에 CurrentLocationInfo 보내기
    // 다른 곳에서의 참조용 변수
    FString GetCurrentZoneName() const;

    // 캐릭터 머리 위에 띄울 이모지
    UPROPERTY(VisibleAnywhere)
    class UWidgetComponent* EmojiUIComp;
    
    //StreamID UserID 레코드액터에 넣기
    AActor* AreaActor;
    UFUNCTION(Server, Reliable)
    void ServerAddUserInfoToRecordActor(AActor* pRecordActor, const FString& pUserID,const FString& pStreamID);
    UFUNCTION(Server, UnReliable)
    void ServerUpdateUserInfoToRecordActor(AActor* pRecordActor, const FString& pUserID,const FString& pStreamID);
     UFUNCTION(Server, Reliable)
    void ServerRemoveUserInfoToRecordActor(AActor* pRecordActor, const FString& pUserID);
private:  
    void SendLocationInfoToServer(FDateTime entry, FDateTime exist, FString zoneName, FString userId, int32 CurrentLocationInformation);

    // 서버 응답 처리
    void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
