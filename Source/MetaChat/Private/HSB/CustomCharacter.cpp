// Fill out your copyright notice in the Description page of Project Settings.


#include "HSB/CustomCharacter.h"
#include "../MetaChat.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SB/CustomSaveGame.h"
#include "Engine/SkeletalMesh.h"
#include "UObject/SoftObjectPtr.h"
#include "../MetaChatPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

#include "Net/UnrealNetwork.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "HSB/CustomAnimInstance.h"
#include "LSJ/MetaChatGameInstance.h"
#include "Engine/TimerHandle.h"
#include "HSB/ChairActor.h"
#include "Components/ArrowComponent.h"
#include "YWK/Recorderactor.h"
#include "LSJ/ScreenActor.h"
#include "Components/WidgetComponent.h"
#include "Materials/MaterialInterface.h"
#include "Engine/AssetManager.h"
#include "Engine/SkinnedAssetCommon.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

// Sets default values
ACustomCharacter::ACustomCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
    SpringArm->TargetArmLength = 800.f;
    SpringArm->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
    SpringArm->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

    // Create a camera...
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

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

    // Don't rotate character to camera direction
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;


    GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
    GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);

    // SpringArm이 캐릭터의 회전을 따라가지 않도록 설정
    if (SpringArm)
    {
        SpringArm->bUsePawnControlRotation = false;
    }

    // 캐릭터 머리 위 이모지
    EmojiUIComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("EmojiUIComp"));
    EmojiUIComp->SetupAttachment(SpringArm);
}

void ACustomCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACustomCharacter, CustomizationData);
}

// Called when the game starts or when spawned
void ACustomCharacter::BeginPlay()
{
	Super::BeginPlay();

    // 캐릭터의 충돌 설정, BeginOverlap과 EndOverlap 바인딩
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ACustomCharacter::OnOverlapBegin);
    GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ACustomCharacter::OnOverlapEnd);
	
    if(this->GetMesh())
    {
		CustomAnimInstance = Cast<UCustomAnimInstance>(GetMesh()->GetAnimInstance());
	}
    CurrentState = ELocationState::IDLE;

    WorldId = 0;
    ZoneName = TEXT("ROOM0");

    bReplicates = true;
    SetReplicateMovement(true);
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

void ACustomCharacter::SetUpLocation(ELocationState State)
{
    if (CurrentState != State)
    {
        CurrentState = State;

        switch (State)
        {
        case ELocationState::IDLE:	Idle();
            break;
        case ELocationState::MOVE:	Move();
            break;
        case ELocationState::SIT:	Sit();
            break;
        default:
            break;
        }
    }
}

void ACustomCharacter::UpdateState()
{
    switch (CurrentState)
    {
    case ELocationState::IDLE:	Idle();
        break;
    case ELocationState::MOVE:	Move();
        break;
    case ELocationState::SIT:	Sit();
        break;
    default:
        break;
    }
}

void ACustomCharacter::Idle()
{
    if (CustomAnimInstance)
    {
        CustomAnimInstance->IsWalking = false;
        CustomAnimInstance->IsSitting = false;
        CustomAnimInstance->WasSit = false;
        CustomAnimInstance->PlayIdleMontage();
    }
}

void ACustomCharacter::Move()
{
    if (CustomAnimInstance)
    {
        CustomAnimInstance->IsWalking = true;
        CustomAnimInstance->IsSitting = false;
        CustomAnimInstance->WasSit = false;
        CustomAnimInstance->PlayWalkMontage();
    }
}

void ACustomCharacter::Sit()
{
    if (CustomAnimInstance)
    {
        CustomAnimInstance->IsWalking = false;
        CustomAnimInstance->IsSitting = true;
        CustomAnimInstance->WasSit = false;
        CustomAnimInstance->PlaySitMontage();

        // 움직임 비활성화하여 캐릭터가 움직이지 못하게 설정
        APlayerController* PlayerController = Cast<APlayerController>(GetController());
        UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
        if (PlayerController && MovementComponent)
        {
            // 2초 동안 움직임 비활성화
            DisableMovement();
            // 2초 후에 움직임 다시 활성화
            GetWorldTimerManager().SetTimer(controlHandle, this, &ACustomCharacter::EnableMovement, 2.f, false);
        }
        // 2초 뒤에 PlaySitIdleMontage() 실행
        GetWorldTimerManager().SetTimer(animHandle, this, &ACustomCharacter::SitIdle, 2.f, false);
    }
}

void ACustomCharacter::DisableMovement()
{
    // 움직임 비활성화하여 캐릭터가 움직이지 못하게 설정
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
    if (PlayerController && MovementComponent)
    {
        // 2초 동안 움직임 비활성화
        DisableInput(PlayerController);
        MovementComponent->DisableMovement();
    }
}

void ACustomCharacter::EnableMovement()
{
    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
    if (PlayerController && MovementComponent)
    {
        // 움직임 활성화
        EnableInput(PlayerController);
        MovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
    }
}

void ACustomCharacter::SitIdle()
{
    if (CustomAnimInstance)
    {
        CustomAnimInstance->PlaySitIdleMontage();
        CustomAnimInstance->IsWalking = false;
        CustomAnimInstance->IsSitting = true;
        CustomAnimInstance->WasSit = true;
    }
}

void ACustomCharacter::ServerAddUserInfoToRecordActor_Implementation(AActor* pRecordActor,const FString& pUserID,const FString& pStreamID)
{
    auto* RecorderActor = Cast<ARecorderactor>(pRecordActor);
	if (RecorderActor)
	{
		if(pUserID.IsEmpty())
			return;
		RecorderActor->AddUser(pUserID,pStreamID);
	}
}

void ACustomCharacter::ServerUpdateUserInfoToRecordActor_Implementation(AActor* pRecordActor, const FString& pUserID, const FString& pStreamID)
{
    auto* RecorderActor = Cast<ARecorderactor>(pRecordActor);
	if (RecorderActor)
	{
		if(pUserID.IsEmpty())
			return;
		RecorderActor->UpdateUser(pUserID,pStreamID);
	}
}

void ACustomCharacter::ServerRemoveUserInfoToRecordActor_Implementation(AActor* pRecordActor, const FString& pUserID)
{
    auto* RecorderActor = Cast<ARecorderactor>(pRecordActor);
	if (RecorderActor)
	{
		if(pUserID.IsEmpty())
			return;
		RecorderActor->RemoveUser(pUserID);
	}
}

void ACustomCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
     // 의자 액터에 overlap 되었는지
    if (OtherActor && OtherActor->ActorHasTag(FName("Chair")))
    {
        AChairActor* Chair = Cast<AChairActor>(OtherActor);

        // 의자에 앉는 애니메이션 작동
        SetUpLocation(ELocationState::SIT);

        // 오버랩 된 액터의 방향과 같은 방향으로 앉도록
        FRotator Rot = OtherActor->GetActorRotation();
        FVector Loc = OtherActor->GetActorLocation();
        Rot.Yaw += 90.f;
        this->SetActorLocationAndRotation(Loc, Rot);


        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Sit on a chair"));
        }
        UE_LOG(LogTemp, Warning, TEXT("Sit"));

    }

    auto* gi = Cast<UMetaChatGameInstance>(GetWorld()->GetGameInstance());

    if (OtherActor && OtherActor->ActorHasTag(FName("Room0")))
    {
        // 현재 위치 정보를 1로 설정
       // 캐릭터가 1에 들어갔을 때 서버로 정보 전송
        EntryTime = FDateTime::Now();
        ExitTime;  // 빈 값으로 처리
        ZoneName = "ROOM0";  // 가정된 존 이름
        UserId = gi->UserID;  // 유저 아이디
        WorldId = gi->WorldID; // 세션 아이디
        AreaActor = OtherActor;

        if (IsLocallyControlled())
        {
            AScreenActor* ScreenActor = Cast<AScreenActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AScreenActor::StaticClass()));
            ServerAddUserInfoToRecordActor(AreaActor, UserId, ScreenActor->UserStreamID);
            GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Black, FString::Printf(TEXT("UserID : %s"), *UserId));
        }
        if (GEngine)
        {
            FString Message = FString::Printf(TEXT("Entered Location Info: %d"), WorldId);
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
        }
        UE_LOG(LogTemp, Warning, TEXT("Room0"));
    }

    // 다른 액터에 "Room1" 태그가 있는지 확인
    if (OtherActor && OtherActor->ActorHasTag(FName("Room1")))
    {
         // 현재 위치 정보를 1로 설정
        // 캐릭터가 1에 들어갔을 때 서버로 정보 전송
        EntryTime = FDateTime::Now();
        ExitTime;  // 빈 값으로 처리
        ZoneName = "ROOM1";  // 가정된 존 이름
        UserId = gi->UserID;  // 유저 아이디
        WorldId = gi->WorldID; // 세션 아이디
        AreaActor = OtherActor;

		if (IsLocallyControlled())
		{
            AScreenActor* ScreenActor =Cast<AScreenActor>(UGameplayStatics::GetActorOfClass(GetWorld(),AScreenActor::StaticClass()));
			ServerAddUserInfoToRecordActor(AreaActor, UserId, ScreenActor->UserStreamID);
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Black, FString::Printf(TEXT("UserID : %s"), *UserId));
		}
         if (GEngine)
         {
             FString Message = FString::Printf(TEXT("Entered Location Info: %d"), WorldId);
             GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
         }
         UE_LOG(LogTemp, Warning, TEXT("Room1"));
    }
    // 다른 액터에 "Room2" 태그가 있는지 확인
    if (OtherActor && OtherActor->ActorHasTag(FName("Room2")))
    {
        // 현재 위치 정보를 2로 설정
        // 캐릭터가 2에 들어갔을 때 서버로 정보 전송
        EntryTime = FDateTime::Now();
        ExitTime;  // 빈 값으로 처리
        ZoneName = "ROOM2";  // 가정된 존 이름
        UserId = gi->UserID;  // 유저 아이디
        WorldId = gi->WorldID; // 세션 아이디
        AreaActor = OtherActor;
		if (IsLocallyControlled())
		{
            AScreenActor* ScreenActor =Cast<AScreenActor>(UGameplayStatics::GetActorOfClass(GetWorld(),AScreenActor::StaticClass()));
			ServerAddUserInfoToRecordActor(AreaActor, UserId, ScreenActor->UserStreamID);
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Black, FString::Printf(TEXT("UserID : %s"), *UserId));
		}
        if (GEngine)
        {
            FString Message = FString::Printf(TEXT("Entered Location Info: %d"), WorldId);
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
        }

    }
    // 다른 액터에 "Room3" 태그가 있는지 확인
    if (OtherActor && OtherActor->ActorHasTag(FName("Room3")))
    {
        // 현재 위치 정보를 3로 설정
        // 캐릭터가 3에 들어갔을 때 서버로 정보 전송
        EntryTime = FDateTime::Now();
        ExitTime;  // 빈 값으로 처리
        ZoneName = "ROOM3";  // 가정된 존 이름
        UserId = gi->UserID;  // 유저 아이디
        WorldId = gi->WorldID; // 세션 아이디       
        AreaActor = OtherActor;
		if (IsLocallyControlled())
		{
            AScreenActor* ScreenActor =Cast<AScreenActor>(UGameplayStatics::GetActorOfClass(GetWorld(),AScreenActor::StaticClass()));
			ServerAddUserInfoToRecordActor(AreaActor, UserId, ScreenActor->UserStreamID);
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Black, FString::Printf(TEXT("UserID : %s"), *UserId));
		}
        if (GEngine)
        {
            FString Message = FString::Printf(TEXT("Entered Location Info: %d"), WorldId);
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
        }

    }
    // 다른 액터에 "Room4" 태그가 있는지 확인
    if (OtherActor && OtherActor->ActorHasTag(FName("Room4")))
    {
        // 현재 위치 정보를 4로 설정
        // 캐릭터가 4에 들어갔을 때 서버로 정보 전송
        EntryTime = FDateTime::Now();
        ExitTime;  // 빈 값으로 처리
        ZoneName = "ROOM4";  // 가정된 존 이름
        UserId = gi->UserID;  // 유저 아이디
        WorldId = gi->WorldID; // 세션 아이디
        AreaActor = OtherActor;
		if (IsLocallyControlled())
		{
            AScreenActor* ScreenActor =Cast<AScreenActor>(UGameplayStatics::GetActorOfClass(GetWorld(),AScreenActor::StaticClass()));
			ServerAddUserInfoToRecordActor(AreaActor, UserId, ScreenActor->UserStreamID);
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Black, FString::Printf(TEXT("UserID : %s"), *UserId));
		}
        if (GEngine)
        {
            FString Message = FString::Printf(TEXT("Entered Location Info: %d"), WorldId);
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
        }
    }
    // 다른 액터에 "Room5" 태그가 있는지 확인
    if (OtherActor && OtherActor->ActorHasTag(FName("Room5")))
    {
        // 현재 위치 정보를 5로 설정
        // 캐릭터가 5에 들어갔을 때 서버로 정보 전송
        EntryTime = FDateTime::Now();
        ExitTime;  // 빈 값으로 처리
        ZoneName = "ROOM5";  // 가정된 존 이름
        UserId = gi->UserID;  // 유저 아이디
        WorldId = gi->WorldID; // 세션 아이디
        AreaActor = OtherActor;
        if (IsLocallyControlled())
        {
            AScreenActor* ScreenActor = Cast<AScreenActor>(UGameplayStatics::GetActorOfClass(GetWorld(), AScreenActor::StaticClass()));
            ServerAddUserInfoToRecordActor(AreaActor, UserId, ScreenActor->UserStreamID);
            GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Black, FString::Printf(TEXT("UserID : %s"), *UserId));
        }
        if (GEngine)
        {
            FString Message = FString::Printf(TEXT("Entered Location Info: %d"), WorldId);
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
        }
    }
 }

void ACustomCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // 의자 액터에서 벗어났을 때
    if (OtherActor && OtherActor->ActorHasTag(FName("Chair")))
    {
        // 다시 기본 애니메이션으로 전환
        SetUpLocation(ELocationState::IDLE);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Stand Up from a chair"));
        }
    }

    auto* gi = Cast<UMetaChatGameInstance>(GetWorld()->GetGameInstance());

    if (OtherActor && OtherActor->ActorHasTag(FName("Room0")))
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Left Room1"));
        }

        // 캐릭터가 Section1을 떠났을 때 서버로 정보 전송
        ExitTime = FDateTime::Now();  // 현재 시간을 ExitTime으로 설정
        ZoneName = "ROOM0";  // 가정된 존 이름
        UserId = gi->UserID;  // 유저 아이디
        WorldId = gi->WorldID; // 세션 아이디
        if (IsLocallyControlled())
            ServerRemoveUserInfoToRecordActor(AreaActor, UserId);
        AreaActor = nullptr;
        // 서버에 정보 전송
        SendLocationInfoToServer(EntryTime, ExitTime, ZoneName, UserId, WorldId);
    }

    // 다른 액터에 "Room1" 태그가 있는지 확인
    if (OtherActor && OtherActor->ActorHasTag(FName("Room1")))
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Left Room1"));
        }

        // 캐릭터가 Section1을 떠났을 때 서버로 정보 전송
        ExitTime = FDateTime::Now();  // 현재 시간을 ExitTime으로 설정
        ZoneName = "ROOM1";  // 가정된 존 이름
        UserId = gi->UserID;  // 유저 아이디
        WorldId = gi->WorldID; // 세션 아이디
        if(IsLocallyControlled())
	        ServerRemoveUserInfoToRecordActor(AreaActor,UserId);
	    AreaActor = nullptr;
        // 서버에 정보 전송
        SendLocationInfoToServer(EntryTime, ExitTime, ZoneName, UserId, WorldId);

        // 서버에 정보 보낸 이후엔 현위치 로비
        ZoneName = "ROOM0";
        UE_LOG(LogTemp, Warning, TEXT("Left Room1, Location Info: %d"), WorldId);
    }
    // 다른 액터에 "Room2" 태그가 있는지 확인
    if (OtherActor && OtherActor->ActorHasTag(FName("Room2")))
    {
        UE_LOG(LogTemp, Warning, TEXT("Left Room2"));

        // 캐릭터가 Section2을 떠났을 때 서버로 정보 전송
        ExitTime = FDateTime::Now();  // 현재 시간을 ExitTime으로 설정
        ZoneName = "ROOM2";  // 가정된 존 이름
        UserId = gi->UserID;  // 유저 아이디
        WorldId = gi->WorldID; // 세션 아이디
        if(IsLocallyControlled())
	        ServerRemoveUserInfoToRecordActor(AreaActor,UserId);
	    AreaActor = nullptr;
        // 서버에 정보 전송
        SendLocationInfoToServer(EntryTime, ExitTime, ZoneName, UserId, WorldId);

        // 서버에 정보 보낸 이후엔 현위치 로비
        ZoneName = "ROOM0";
        UE_LOG(LogTemp, Warning, TEXT("Left Section2, Location Info: %d"), WorldId);
    }
    // 다른 액터에 "Room3" 태그가 있는지 확인
    if (OtherActor && OtherActor->ActorHasTag(FName("Room3")))
    {
        UE_LOG(LogTemp, Warning, TEXT("Left Room3"));

        // 캐릭터가 Section3을 떠났을 때 서버로 정보 전송
        ExitTime = FDateTime::Now();  // 현재 시간을 ExitTime으로 설정
        ZoneName = "ROOM3";  // 가정된 존 이름
        UserId = gi->UserID;  // 유저 아이디
        WorldId = gi->WorldID; // 세션 아이디
        if(IsLocallyControlled())
	       ServerRemoveUserInfoToRecordActor(AreaActor,UserId);
	    AreaActor = nullptr; 
        // 서버에 정보 전송
        SendLocationInfoToServer(EntryTime, ExitTime, ZoneName, UserId, WorldId);

        // 서버에 정보 보낸 이후엔 현위치 로비
        ZoneName = "ROOM0";
        UE_LOG(LogTemp, Warning, TEXT("Left Section3, Location Info: %d"), WorldId);
    }
    // 다른 액터에 "Room4" 태그가 있는지 확인
    if (OtherActor && OtherActor->ActorHasTag(FName("Room4")))
    {
        UE_LOG(LogTemp, Warning, TEXT("Left Room4"));

        // 캐릭터가 Section4을 떠났을 때 서버로 정보 전송
        ExitTime = FDateTime::Now();  // 현재 시간을 ExitTime으로 설정
        ZoneName = "ROOM4";  // 가정된 존 이름
        UserId = gi->UserID;  // 유저 아이디
        WorldId = gi->WorldID; // 세션 아이디
        if(IsLocallyControlled())
	       ServerRemoveUserInfoToRecordActor(AreaActor,UserId);
	    AreaActor = nullptr;
        // 서버에 정보 전송
        SendLocationInfoToServer(EntryTime, ExitTime, ZoneName, UserId, WorldId);
        
        // 서버에 정보 보낸 이후엔 현위치 로비
        ZoneName = "ROOM0";
        UE_LOG(LogTemp, Warning, TEXT("Left Section4, Location Info: %d"), WorldId);
    }
    // 다른 액터에 "Room5" 태그가 있는지 확인
    if (OtherActor && OtherActor->ActorHasTag(FName("Room5")))
    {
        UE_LOG(LogTemp, Warning, TEXT("Left Room5"));

        // 캐릭터가 Section4을 떠났을 때 서버로 정보 전송
        ExitTime = FDateTime::Now();  // 현재 시간을 ExitTime으로 설정
        ZoneName = "ROOM5";  // 가정된 존 이름
        UserId = gi->UserID;  // 유저 아이디
        WorldId = gi->WorldID; // 세션 아이디
        if (IsLocallyControlled())
            ServerRemoveUserInfoToRecordActor(AreaActor, UserId);
        AreaActor = nullptr;
        // 서버에 정보 전송
        SendLocationInfoToServer(EntryTime, ExitTime, ZoneName, UserId, WorldId);

        // 서버에 정보 보낸 이후엔 현위치 로비
        ZoneName = "ROOM0";
        UE_LOG(LogTemp, Warning, TEXT("Left Section5, Location Info: %d"), WorldId);
    }
 }

 void ACustomCharacter::Load()
 {
    // 저장된 데이터를 로드하고 캐릭터에 반영
    UCustomSaveGame* LoadedGameInstance = Cast<UCustomSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("CharacterMeshSaveSlot"), 0));
    if (LoadedGameInstance)
    {
        FString SaveGamePath = FPaths::ProjectSavedDir();  // Save 디렉토리 경로 확인
        UE_LOG(LogTemp, Log, TEXT("Save game loaded from: %s"), *SaveGamePath);

        // 레벨 들어가면 메쉬의 material 변경됨
        UWorld* World = GetWorld();
        if (!World)
            return;
        FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(World);
        if (CurrentLevelName == "AlphaMain")
        {
            // SavedMeshes에서 데이터를 가져와 FCharacterCustomizationData 구조체로 변환

            // HairMesh 로드 및 적용
            FString* MeshPath_h = LoadedGameInstance->SavedMeshes.Find("Hair");
            UE_LOG(LogTemp, Warning, TEXT("%s"), **MeshPath_h);

            if (MeshPath_h)
            {
                USkeletalMesh* LoadedMesh = LoadObject<USkeletalMesh>(nullptr, **MeshPath_h);
                if (LoadedMesh)
                {
                    // Hair 머티리얼 설정 (첫 번째 머티리얼 슬롯에 설정)
                    //여자 헤어
                    UMaterialInterface* NewHairMaterial1 = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/XR_HSB/Character/Hair_Long_v001_Mt1.Hair_Long_v001_Mt1"));
                    //남자 헤어
                    UMaterialInterface* NewHairMaterial2 = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/XR_HSB/Character/Hair_mt1.Hair_mt1"));
                    // GetMaterials()는 FSkeletalMaterial 배열을 반환함
                    const TArray<FSkeletalMaterial>& Materials = LoadedMesh->GetMaterials();
                    
                    for (int32 i = 0; i < Materials.Num(); ++i)
                    {
                        const FSkeletalMaterial& MaterialSlot = Materials[i];

                        // MaterialInterface로 실제 UMaterialInstance나 UMaterial를 가져올 수 있음
                        if (MaterialSlot.MaterialInterface == NewHairMaterial1) //여자헤어면 LoadedMesh에 넣기
                        {
                            UMaterialInterface* NewHairMaterial3 = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/TA_JSG/Character/Material/Hair_Long_v001_Mt.Hair_Long_v001_Mt"));
                            if (NewHairMaterial3 && LoadedMesh->Materials.IsValidIndex(0))
                            {
                                LoadedMesh->Materials[0] = NewHairMaterial3;
                                ServerSetSkeletalMesh(LoadedMesh, NewHairMaterial3, "Hair");
                            }
                        }
                        else  if (MaterialSlot.MaterialInterface == NewHairMaterial2) //남자헤어면 LoadedMesh에 넣기
                        {
                            UMaterialInterface* NewHairMaterial3 = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/TA_JSG/Character/Material/Hair_mt.Hair_mt"));
                            if (NewHairMaterial3 && LoadedMesh->Materials.IsValidIndex(0))
                            {
                                LoadedMesh->Materials[0] = NewHairMaterial3;
                                ServerSetSkeletalMesh(LoadedMesh, NewHairMaterial3, "Hair");
                            }
                        }
                    }
                }
            }
            // Upper 머티리얼 설정 (첫 번째 머티리얼 슬롯에 설정)
            // UpperBodyMesh 로드 및 적용
            FString* MeshPath_u = LoadedGameInstance->SavedMeshes.Find("Upper");
            if (MeshPath_u)
            {
                USkeletalMesh* LoadedMesh = LoadObject<USkeletalMesh>(nullptr, **MeshPath_u);
                if (LoadedMesh)
                {
                    // SavedMeshes에서 데이터를 가져와 FCharacterCustomizationData 구조체로 변환

                    // LoadedMesh의 경로를 가져와 비교
                    FString LoadedMeshPath = LoadedMesh->GetPathName();
                    UE_LOG(LogTemp, Warning, TEXT("Loaded Mesh Path: %s"), *LoadedMeshPath);

                    //상의 1
                    UMaterialInterface* NewUpperMaterial1 = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/XR_HSB/Character/Player_Cloth_LongTop_Mt1.Player_Cloth_LongTop_Mt1"));
                    //상의 2
                    UMaterialInterface* NewUpperMaterial2 = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/XR_HSB/Character/Cloth_Top_Short_Base_color_Mat1.Cloth_Top_Short_Base_color_Mat1"));
                    // GetMaterials()는 FSkeletalMaterial 배열을 반환함
                    const TArray<FSkeletalMaterial>& Materials = LoadedMesh->GetMaterials();
                    
                    for (int32 i = 0; i < Materials.Num(); ++i)
                    {
                        const FSkeletalMaterial& MaterialSlot = Materials[i];
                        
                        // MaterialInterface로 실제 UMaterialInstance나 UMaterial를 가져올 수 있음
                        if (MaterialSlot.MaterialInterface == NewUpperMaterial1)
                        {
                            UMaterialInterface* NewUpperMaterial3 = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/TA_JSG/Character/Material/Player_Cloth_LongTop_Mt.Player_Cloth_LongTop_Mt"));
                            if (NewUpperMaterial3 && LoadedMesh->Materials.IsValidIndex(0))
                            {
                                LoadedMesh->Materials[0] = NewUpperMaterial3;
                                ServerSetSkeletalMesh(LoadedMesh, NewUpperMaterial3, "Upper");
                            }
                        }
                        else  if (MaterialSlot.MaterialInterface == NewUpperMaterial2)
                        {
                            UMaterialInterface* NewUpperMaterial3 = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/TA_JSG/Character/Material/Cloth_Top_Short_Base_color_Mat.Cloth_Top_Short_Base_color_Mat"));
                            if (NewUpperMaterial3 && LoadedMesh->Materials.IsValidIndex(0))
                            {
                                LoadedMesh->Materials[0] = NewUpperMaterial3;
                                ServerSetSkeletalMesh(LoadedMesh, NewUpperMaterial3, "Upper");
                            }
                        }
                    }
                }
            }

            
            // Lower 머티리얼 설정 (첫 번째 머티리얼 슬롯에 설정)
            // LowerBodyMesh 로드 및 적용
            FString* MeshPath_l = LoadedGameInstance->SavedMeshes.Find("Lower");
            if (MeshPath_l)
            {
                USkeletalMesh* LoadedMesh = LoadObject<USkeletalMesh>(nullptr, **MeshPath_l);
                if (LoadedMesh)
                {
                    // SavedMeshes에서 데이터를 가져와 FCharacterCustomizationData 구조체로 변환
                    FString LoadedMeshPath = LoadedMesh->GetPathName();

                    UMaterialInterface* NewLowerMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/TA_JSG/Character/Material/Cloth_Pants.Cloth_Pants"));
                    FString AssetLowerPath = TEXT("/Game/TA_JSG/Character/Material/Cloth_Pants.Cloth_Pants");
                    FSoftObjectPath LowerSoftObjectPath(AssetLowerPath);
                    UAssetManager::GetStreamableManager().RequestAsyncLoad(LowerSoftObjectPath, [this, LowerSoftObjectPath, LoadedMesh]()
                    {
                        UMaterialInterface* LoadedMat = Cast<UMaterialInterface>(LowerSoftObjectPath.TryLoad());
                        if (LoadedMat)
                        {
                            // Materials 배열의 첫 번째 슬롯에 새로운 머티리얼을 설정
                            if (LoadedMesh->Materials.IsValidIndex(0))
                            {
                                LoadedMesh->Materials[0] = LoadedMat;
                                ServerSetSkeletalMesh(LoadedMesh, LoadedMat, "Lower");
                            }
                            UE_LOG(LogTemp, Warning, TEXT("Success to load Lower Material dynamically."));
                        }
                        else
                        {
                            UE_LOG(LogTemp, Error, TEXT("Failed to load Lower Material dynamically."));
                        }
                    });
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
                    FString LoadedMeshPath = LoadedMesh->GetPathName();
                    // Shoes 머티리얼 설정 (첫 번째 머티리얼 슬롯에 설정)
                    UMaterialInterface* NewShoesMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/TA_JSG/Character/Material/Cloth_Shoes.Cloth_Shoes"));
                    FString AssetShoesPath = TEXT("/Game/TA_JSG/Character/Material/Cloth_Shoes.Cloth_Shoes");
                    FSoftObjectPath ShoesSoftObjectPath(AssetShoesPath);
                    UAssetManager::GetStreamableManager().RequestAsyncLoad(ShoesSoftObjectPath, [this, ShoesSoftObjectPath, LoadedMesh]()
                    {
                        UMaterialInterface* LoadedMat = Cast<UMaterialInterface>(ShoesSoftObjectPath.TryLoad());
                        if (LoadedMat)
                        {
                            // Materials 배열의 첫 번째 슬롯에 새로운 머티리얼을 설정
                            if (LoadedMesh->Materials.IsValidIndex(0))
                            {
                                LoadedMesh->Materials[0] = LoadedMat;
                                ServerSetSkeletalMesh(LoadedMesh, LoadedMat, "Feet");
                            }
                            UE_LOG(LogTemp, Warning, TEXT("Success to load Feet Material dynamically."));
                        }
                        else
                        {
                            UE_LOG(LogTemp, Error, TEXT("Failed to load Feet Material dynamically."));
                        }
                    });
                }
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load save game"));
    }
}

void ACustomCharacter::ServerSetSkeletalMesh_Implementation(USkeletalMesh* NewMesh, UMaterialInterface* NewMat, FName MeshCategory)
{
    // 메쉬를 서버에서 업데이트
    if (MeshCategory == "Hair" && HairMeshComp)
    {
        HairMeshComp->SetMaterial(0, NewMat);
        HairMeshComp->SetSkeletalMesh(NewMesh);
        CustomizationData.HairMesh = NewMesh;
    }
    if (MeshCategory == "Upper" && UpperBodyMeshComp)
    {
        UpperBodyMeshComp->SetMaterial(0, NewMat);
        UpperBodyMeshComp->SetSkeletalMesh(NewMesh);
        CustomizationData.UpperBodyMesh = NewMesh;
    }
    if (MeshCategory == "Lower" && LowerBodyMeshComp)
    {
        LowerBodyMeshComp->SetMaterial(0, NewMat);
        LowerBodyMeshComp->SetSkeletalMesh(NewMesh);
        CustomizationData.LowerBodyMesh = NewMesh;
    }
    if (MeshCategory == "Feet" && FeetMeshComp)
    {
        FeetMeshComp->SetMaterial(0, NewMat);
        FeetMeshComp->SetSkeletalMesh(NewMesh);
        CustomizationData.FeetMesh = NewMesh;
    }

    // 모든 클라이언트에 이 변경사항을 브로드캐스트
     MulticastUpdateSkeletalMesh(NewMesh, NewMat, MeshCategory);
}

void ACustomCharacter::MulticastUpdateSkeletalMesh_Implementation(USkeletalMesh* NewMesh, UMaterialInterface* NewMat, FName MeshCategory)
{
    // 모든 클라이언트에서 메쉬 업데이트
    if (MeshCategory == "Hair" && HairMeshComp)
    {
        HairMeshComp->SetMaterial(0, NewMat);
        HairMeshComp->SetSkeletalMesh(NewMesh);
    }
    if (MeshCategory == "Upper" && UpperBodyMeshComp)
    {
        UpperBodyMeshComp->SetMaterial(0, NewMat);
        UpperBodyMeshComp->SetSkeletalMesh(NewMesh);
    }
    if (MeshCategory == "Lower" && LowerBodyMeshComp)
    {
        LowerBodyMeshComp->SetMaterial(0, NewMat);
        LowerBodyMeshComp->SetSkeletalMesh(NewMesh);
    }
    if (MeshCategory == "Feet" && FeetMeshComp)
    {
        FeetMeshComp->SetMaterial(0, NewMat);
        FeetMeshComp->SetSkeletalMesh(NewMesh);
    }

    UpdateState();
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
        UpperBodyMeshComp->SetSkeletalMesh(CustomizationData.UpperBodyMesh);
    }
    if (CustomizationData.LowerBodyMesh)
    {
        LowerBodyMeshComp->SetSkeletalMesh(CustomizationData.LowerBodyMesh);
    }
    if (CustomizationData.FeetMesh)
    {
        FeetMeshComp->SetSkeletalMesh(CustomizationData.FeetMesh);
    }
}

// ------------- 회의 구역 정보 서버에 전달 -----------------
FString ACustomCharacter::GetCurrentZoneName() const
{
    return ZoneName;  // 현재 ZoneName을 반환
}

void ACustomCharacter::SendLocationInfoToServer(FDateTime entry, FDateTime exist, FString zoneName, FString userId, int32 worldId)
{
    // HTTP 모듈 초기화
    FHttpModule* Http = &FHttpModule::Get();
    if (!Http)
    {
        UE_LOG(LogTemp, Warning, TEXT("HTTP module is not available"));
        return;
    }

    // 요청 생성
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

    // 서버 URL 설정 (여기에 실제 서버 주소를 넣어주세요)
    Request->SetURL(TEXT("http://125.132.216.190:8126/api/zoneHistory"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // FDateTime을 문자열로 변환 (ISO8601 형식 등)
    FString EntryTimeString = entry.ToString(TEXT("%Y-%m-%dT%H:%M:%S"));
    FString ExitTimeString = exist.ToString(TEXT("%Y-%m-%dT%H:%M:%S"));

    // JSON 데이터 생성
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    JsonObject->SetStringField(TEXT("entryTime"), EntryTimeString);
    JsonObject->SetStringField(TEXT("existTime"), ExitTimeString);
    JsonObject->SetStringField(TEXT("zoneName"), zoneName);
    JsonObject->SetStringField(TEXT("userId"), userId);
    JsonObject->SetNumberField(TEXT("worldId"), worldId);


    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    Request->SetContentAsString(RequestBody);

    // 응답 처리 바인딩
    Request->OnProcessRequestComplete().BindUObject(this, &ACustomCharacter::OnResponseReceived);

    UE_LOG(LogTemp, Warning, TEXT("Entered Section1, Location Info: %d, entryTime: %s, existTime: %s, zoneName: %s, UserID: %s"), worldId, *EntryTimeString, *ExitTimeString, *zoneName, *userId);

    // 요청 전송
    if (!Request->ProcessRequest())
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to send HTTP request"));
    }
}

void ACustomCharacter::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{  
    if (bWasSuccessful && Response.IsValid())
    {
        // 서버에서 온 응답 처리
        FString ResponseContent = Response->GetContentAsString();
        UE_LOG(LogTemp, Log, TEXT("Response from server: %s"), *ResponseContent);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to receive valid response from server"));
    }
}