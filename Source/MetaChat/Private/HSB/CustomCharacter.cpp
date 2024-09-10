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

    CurrentLocationInfo = 0;
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
    default:
        break;
    }
}

void ACustomCharacter::Idle()
{
    if (CustomAnimInstance)
    {
        CustomAnimInstance->IsWalking = false;
        CustomAnimInstance->PlayIdleMontage();
    }
}

void ACustomCharacter::Move()
{
    if (CustomAnimInstance)
    {
        CustomAnimInstance->IsWalking = true;
        CustomAnimInstance->PlayWalkMontage();
    }
}

void ACustomCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
       // 의자 액터에 overlap 되었는지
    if (OtherActor && OtherActor->ActorHasTag(FName("Chair")))
    {
        // 의자에 앉는 애니메이션 작동

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Sit on a chair"));
        }
        UE_LOG(LogTemp, Warning, TEXT("Sit"));

    }

    // 다른 액터에 "Room1" 태그가 있는지 확인
    if (OtherActor && OtherActor->ActorHasTag(FName("Room1")))
    {
         // 현재 위치 정보를 1로 설정
        // 캐릭터가 1에 들어갔을 때 서버로 정보 전송
        FDateTime EntryTime = FDateTime::Now();
        FDateTime ExitTime;  // 빈 값으로 처리
        FName ZoneName = "ROOM1";  // 가정된 존 이름
        FString UserId = "User123";  // 가정된 유저 아이디
        CurrentLocationInfo = 1;  // 예시로 1

        // 서버에 정보 전송
        SendLocationInfoToServer(EntryTime, ExitTime, ZoneName, UserId, CurrentLocationInfo);


         if (GEngine)
         {
             FString Message = FString::Printf(TEXT("Entered Location Info: %d"), CurrentLocationInfo);
             GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
         }
         UE_LOG(LogTemp, Warning, TEXT("Room1"));
    }
    // 다른 액터에 "Room2" 태그가 있는지 확인
    if (OtherActor && OtherActor->ActorHasTag(FName("Room2")))
    {
        // 현재 위치 정보를 2로 설정
        // 캐릭터가 2에 들어갔을 때 서버로 정보 전송
        FDateTime EntryTime = FDateTime::Now();
        FDateTime ExitTime;  // 빈 값으로 처리
        FName ZoneName = "ROOM2";  // 가정된 존 이름
        FString UserId = "User123";  // 가정된 유저 아이디
        CurrentLocationInfo = 2;  // 예시로 2

        // 서버에 정보 전송
        SendLocationInfoToServer(EntryTime, ExitTime, ZoneName, UserId, CurrentLocationInfo);


        if (GEngine)
        {
            FString Message = FString::Printf(TEXT("Entered Location Info: %d"), CurrentLocationInfo);
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
        }

    }
    // 다른 액터에 "Room3" 태그가 있는지 확인
    if (OtherActor && OtherActor->ActorHasTag(FName("Room3")))
    {
        // 현재 위치 정보를 3로 설정
        // 캐릭터가 3에 들어갔을 때 서버로 정보 전송
        FDateTime EntryTime = FDateTime::Now();
        FDateTime ExitTime;  // 빈 값으로 처리
        FName ZoneName = "ROOM3";  // 가정된 존 이름
        FString UserId = "User123";  // 가정된 유저 아이디
        CurrentLocationInfo = 3;  

        // 서버에 정보 전송
        SendLocationInfoToServer(EntryTime, ExitTime, ZoneName, UserId, CurrentLocationInfo);
       

        if (GEngine)
        {
            FString Message = FString::Printf(TEXT("Entered Location Info: %d"), CurrentLocationInfo);
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
        }

    }
    // 다른 액터에 "Room4" 태그가 있는지 확인
    if (OtherActor && OtherActor->ActorHasTag(FName("Room4")))
    {
        // 현재 위치 정보를 4로 설정
        // 캐릭터가 4에 들어갔을 때 서버로 정보 전송
        FDateTime EntryTime = FDateTime::Now();
        FDateTime ExitTime;  // 빈 값으로 처리
        FName ZoneName = "ROOM4";  // 가정된 존 이름
        FString UserId = "User123";  // 가정된 유저 아이디
        CurrentLocationInfo = 4; 

        // 서버에 정보 전송
        SendLocationInfoToServer(EntryTime, ExitTime, ZoneName, UserId, CurrentLocationInfo);


        if (GEngine)
        {
            FString Message = FString::Printf(TEXT("Entered Location Info: %d"), CurrentLocationInfo);
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

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Stand Up from a chair"));
        }
    }

    // 다른 액터에 "Room1" 태그가 있는지 확인
    if (OtherActor && OtherActor->ActorHasTag(FName("Room1")))
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Left Room1"));
        }

        // 캐릭터가 Section1을 떠났을 때 서버로 정보 전송
        FDateTime EntryTime;  // 빈 값으로 처리
        FDateTime ExitTime = FDateTime::Now();  // 현재 시간을 ExitTime으로 설정
        FName ZoneName = "ROOM1";  // 가정된 존 이름
        FString UserId = "User123";  // 가정된 유저 아이디
        CurrentLocationInfo = 0;  // 예시로 0

        // 서버에 정보 전송
        SendLocationInfoToServer(EntryTime, ExitTime, ZoneName, UserId, CurrentLocationInfo);

        UE_LOG(LogTemp, Warning, TEXT("Left Room1, Location Info: %d"), CurrentLocationInfo);
    }
    // 다른 액터에 "Room2" 태그가 있는지 확인
    if (OtherActor && OtherActor->ActorHasTag(FName("Room2")))
    {
        UE_LOG(LogTemp, Warning, TEXT("Left Room2"));

        // 캐릭터가 Section1을 떠났을 때 서버로 정보 전송
        FDateTime EntryTime;  // 빈 값으로 처리
        FDateTime ExitTime = FDateTime::Now();  // 현재 시간을 ExitTime으로 설정
        FName ZoneName = "ROOM2";  // 가정된 존 이름
        FString UserId = "User123";  // 가정된 유저 아이디
        CurrentLocationInfo = 0;  // 예시로 0

        // 서버에 정보 전송
        SendLocationInfoToServer(EntryTime, ExitTime, ZoneName, UserId, CurrentLocationInfo);

        UE_LOG(LogTemp, Warning, TEXT("Left Section1, Location Info: %d"), CurrentLocationInfo);
    }
    // 다른 액터에 "Room3" 태그가 있는지 확인
    if (OtherActor && OtherActor->ActorHasTag(FName("Room3")))
    {
        UE_LOG(LogTemp, Warning, TEXT("Left Room3"));

        // 캐릭터가 Section1을 떠났을 때 서버로 정보 전송
        FDateTime EntryTime;  // 빈 값으로 처리
        FDateTime ExitTime = FDateTime::Now();  // 현재 시간을 ExitTime으로 설정
        FName ZoneName = "ROOM2";  // 가정된 존 이름
        FString UserId = "User123";  // 가정된 유저 아이디
        CurrentLocationInfo = 0;  // 예시로 0

        // 서버에 정보 전송
        SendLocationInfoToServer(EntryTime, ExitTime, ZoneName, UserId, CurrentLocationInfo);

        UE_LOG(LogTemp, Warning, TEXT("Left Section1, Location Info: %d"), CurrentLocationInfo);
    }
    // 다른 액터에 "Room4" 태그가 있는지 확인
    if (OtherActor && OtherActor->ActorHasTag(FName("Room4")))
    {
        UE_LOG(LogTemp, Warning, TEXT("Left Room4"));

        // 캐릭터가 Section1을 떠났을 때 서버로 정보 전송
        FDateTime EntryTime;  // 빈 값으로 처리
        FDateTime ExitTime = FDateTime::Now();  // 현재 시간을 ExitTime으로 설정
        FName ZoneName = "ROOM2";  // 가정된 존 이름
        FString UserId = "User123";  // 가정된 유저 아이디
        CurrentLocationInfo = 0;  // 예시로 0

        // 서버에 정보 전송
        SendLocationInfoToServer(EntryTime, ExitTime, ZoneName, UserId, CurrentLocationInfo);
        
        UE_LOG(LogTemp, Warning, TEXT("Left Section1, Location Info: %d"), CurrentLocationInfo);
    }
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
          
                ServerSetSkeletalMesh(LoadedMesh, "Hair");
            
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
//                 UpperBodyMeshComp->SetSkeletalMesh(LoadedMesh);
                ServerSetSkeletalMesh(LoadedMesh, "Upper");
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
//                 LowerBodyMeshComp->SetSkeletalMesh(LoadedMesh);
                ServerSetSkeletalMesh(LoadedMesh, "Lower");
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
//                 FeetMeshComp->SetSkeletalMesh(LoadedMesh);
                ServerSetSkeletalMesh(LoadedMesh, "Feet");
                UE_LOG(LogTemp, Warning, TEXT("Feet mesh has been applied from save."));
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load save game"));
    }
}

void ACustomCharacter::ServerSetSkeletalMesh_Implementation(USkeletalMesh* NewMesh, FName MeshCategory)
{
    // 메쉬를 서버에서 업데이트
    if (MeshCategory == "Hair" && HairMeshComp)
    {
        HairMeshComp->SetSkeletalMesh(NewMesh);
        CustomizationData.HairMesh = NewMesh;
    }
    if (MeshCategory == "Upper" && UpperBodyMeshComp)
    {
        UpperBodyMeshComp->SetSkeletalMesh(NewMesh);
        CustomizationData.UpperBodyMesh = NewMesh;
    }
    if (MeshCategory == "Lower" && LowerBodyMeshComp)
    {
        LowerBodyMeshComp->SetSkeletalMesh(NewMesh);
        CustomizationData.LowerBodyMesh = NewMesh;
    }
    if (MeshCategory == "Shoes" && FeetMeshComp)
    {
        FeetMeshComp->SetSkeletalMesh(NewMesh);
        CustomizationData.FeetMesh = NewMesh;
    }

    // 모든 클라이언트에 이 변경사항을 브로드캐스트
     MulticastUpdateSkeletalMesh(NewMesh, MeshCategory);
}

void ACustomCharacter::MulticastUpdateSkeletalMesh_Implementation(USkeletalMesh* NewMesh, FName MeshCategory)
{
    // 모든 클라이언트에서 메쉬 업데이트
    if (MeshCategory == "Hair" && HairMeshComp)
    {
        HairMeshComp->SetSkeletalMesh(NewMesh);
    }
    if (MeshCategory == "Upper" && UpperBodyMeshComp)
    {
        UpperBodyMeshComp->SetSkeletalMesh(NewMesh);
    }
    if (MeshCategory == "Lower" && LowerBodyMeshComp)
    {
        LowerBodyMeshComp->SetSkeletalMesh(NewMesh);
    }
    if (MeshCategory == "Shoes" && FeetMeshComp)
    {
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
void ACustomCharacter::SendLocationInfoToServer(FDateTime entry, FDateTime exist, FName zoneName, FString userId, int32 CurrentLocationInformation)
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
    JsonObject->SetStringField(TEXT("zoneName"), zoneName.ToString());
    JsonObject->SetStringField(TEXT("userId"), userId);
    JsonObject->SetNumberField(TEXT("worldId"), CurrentLocationInformation);


    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    Request->SetContentAsString(RequestBody);

    // 응답 처리 바인딩
    Request->OnProcessRequestComplete().BindUObject(this, &ACustomCharacter::OnResponseReceived);

    UE_LOG(LogTemp, Warning, TEXT("Entered Section1, Location Info: %d, entryTime: %s, existTime: %s, zoneName: %s, UserID: %s"), CurrentLocationInformation, *EntryTimeString, *ExitTimeString, *zoneName.ToString(), *userId);

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