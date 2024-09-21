#include "HSB/InvSlotWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SkeletalMeshComponent.h"
#include "Delegates/Delegate.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Texture2D.h"
#include "HSB/CustomCharacter.h"
#include "HSB/CustomWidget.h"
#include "Internationalization/Text.h"
#include "Kismet/GameplayStatics.h"
#include <Components/Overlay.h>
#include "Components/OverlaySlot.h"
#include "UObject/SoftObjectPtr.h"
#include "Engine/AssetManager.h"
#include "UObject/NoExportTypes.h"

void UInvSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 버튼 클릭 이벤트 바인딩
    if ( Button_ItemIcon_1 )
    {
        Button_ItemIcon_1->OnClicked.RemoveAll(this); // 기존 바인딩 제거
        Button_ItemIcon_1->OnClicked.AddDynamic(this , &UInvSlotWidget::OnItemClicked);
    }
}

void UInvSlotWidget::SetItemData(const TArray<FSlot>& ItemsData)
{
	// UI 컴포넌트에 데이터 반영
    for ( int32 i = 0; i < ItemsData.Num(); ++i )
    {
        // 동적 위젯 이름 생성
        FString WidgetName = FString::Printf(TEXT("Image_ItemIcon_%d") , i + 1);
        UImage* ImageWidget = Cast<UImage>(GetWidgetFromName(FName(*WidgetName)));
        // 버튼 위젯 동적 생성
        UButton* ButtonWidget = NewObject<UButton>(this , FName(*FString::Printf(TEXT("Button_ItemIcon_%d") , i + 1)));

        if ( ImageWidget )
        {
            // 아이콘 이미지 업데이트
            ImageWidget->SetBrushFromTexture(ItemsData[i].ItemIcon);
            // 버튼 클릭 이벤트 바인딩
            ButtonWidget->OnClicked.AddDynamic(this , &UInvSlotWidget::OnItemClicked);
        }
    }

	if(ItemsData.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetItemData called with empty ItemsData."));
	}
}

void UInvSlotWidget::OnItemClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("OnItemClicked called"));

    // 카테고리를 구분한다
    Character = CastChecked<ACustomCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld() , 0));
    // Mesh 설정할 캐릭터의 SkeletalMeshComponent
    USkeletalMeshComponent* TargetMeshComponent = nullptr;
    if(Character && Character->HasAuthority())
	{
        UE_LOG(LogTemp, Warning, TEXT("Authority confirmed"));

        if(Category == "" )
            UE_LOG(LogTemp , Warning , TEXT("Problem in Loading Category."));

        // 버튼 이름 가져오기
        FString ButtonName = GetName();
        UE_LOG(LogTemp, Warning, TEXT("%s"), *ButtonName);
        ////////////////////////////////////////////////////////////////////////////////////////////
        if (Category == "Hair")
        {
            // 해당 그림에 맞는 Skeletal mesh를 찾는다

            TargetMeshComponent = Character->HairMeshComp; // 캐릭터의 Skeletal Mesh 참조

            FString ExpectedButtonName0 = FString::Printf(TEXT("WBP_InvParts_C_%d"), clickcnt - 2);
            FString ExpectedButtonName1 = FString::Printf(TEXT("WBP_InvParts_C_%d"), clickcnt - 1);

            TSoftObjectPtr<USkeletalMesh> MeshAsset;

            //if ( ButtonName.Equals(ExpectedButtonName0) )
            if (clickcnt == 0)
            {
                USkeletalMesh* MeshAsset1 = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/XR_HSB/Character/Hair_Long_v1"));
                FString AssetPath = TEXT("/Game/XR_HSB/Character/Hair_Long_v1");
                FSoftObjectPath SoftObjectPath(AssetPath);
                UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftObjectPath, [this, SoftObjectPath]()
                {
                    USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(SoftObjectPath.TryLoad());
                    if (LoadedMesh)
                    {
                        Character->HairMeshComp->SetSkeletalMesh(LoadedMesh);
                        UE_LOG(LogTemp, Warning, TEXT("Success to load Mesh dynamically."));
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("Failed to load SkeletalMesh dynamically."));
                        }
                });
            }
            //else if ( ButtonName.Equals(ExpectedButtonName1) )
            else if (clickcnt == 1)
            {
                USkeletalMesh* MeshAsset1 = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/XR_HSB/Character/Hair_Short_"));
                FString AssetPath = TEXT("/Game/XR_HSB/Character/Hair_Short_");
                FSoftObjectPath SoftObjectPath(AssetPath);
                UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftObjectPath, [this, SoftObjectPath]()
                {
                    USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(SoftObjectPath.TryLoad());
                    if (LoadedMesh)
                    {
                        Character->HairMeshComp->SetSkeletalMesh(LoadedMesh);
                        UE_LOG(LogTemp, Warning, TEXT("Success to load Mesh dynamically."));
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("Failed to load SkeletalMesh dynamically."));
                    }
                });
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to Name dynamically."));
                UE_LOG(LogTemp, Error, TEXT("%d %s %s %s"), clickcnt, *ButtonName, *ExpectedButtonName0, *ExpectedButtonName1);
            }
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        else if ( Category == "Upper" )
        {
            // 해당 그림에 맞는 Skeletal mesh를 찾는다

            TargetMeshComponent = Character->UpperBodyMeshComp; // 캐릭터의 Skeletal Mesh 참조

            FString ExpectedButtonName0 = FString::Printf(TEXT("WBP_InvParts_C_%d"), clickcnt - 2);
            FString ExpectedButtonName1 = FString::Printf(TEXT("WBP_InvParts_C_%d"), clickcnt - 1);

            TSoftObjectPtr<USkeletalMesh> MeshAsset;

            if (clickcnt == 0)
            {
                USkeletalMesh* MeshAsset1 = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/XR_HSB/Character/Player_Top_Long.Player_Top_Long"));
                FString AssetPath = TEXT("/Game/XR_HSB/Character/Player_Top_Long.Player_Top_Long");
                FSoftObjectPath SoftObjectPath(AssetPath);
                UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftObjectPath, [this, SoftObjectPath]()
                {
                    USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(SoftObjectPath.TryLoad());
                    if (LoadedMesh)
                    {
                        Character->UpperBodyMeshComp->SetSkeletalMesh(LoadedMesh);
                        UE_LOG(LogTemp, Warning, TEXT("Success to load Mesh dynamically."));
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("Failed to load SkeletalMesh dynamically."));
                    }
                });
            }
            else if (clickcnt == 1)
            {
                USkeletalMesh* MeshAsset1 = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/XR_HSB/Character/Player_Top_Short.Player_Top_Short"));
                FString AssetPath = TEXT("/Game/XR_HSB/Character/Player_Top_Short.Player_Top_Short");
                FSoftObjectPath SoftObjectPath(AssetPath);
                UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftObjectPath, [this, SoftObjectPath]()
                {
                    USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(SoftObjectPath.TryLoad());
                    if (LoadedMesh)
                    {
                        Character->UpperBodyMeshComp->SetSkeletalMesh(LoadedMesh);
                        UE_LOG(LogTemp, Warning, TEXT("Success to load Mesh dynamically."));
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("Failed to load SkeletalMesh dynamically."));
                    }
                });
            }
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        else if ( Category == "Lower" )
        {
            // 해당 그림에 맞는 Skeletal mesh를 찾는다

            TargetMeshComponent = Character->LowerBodyMeshComp; // 캐릭터의 Skeletal Mesh 참조

            FString ExpectedButtonName0 = FString::Printf(TEXT("WBP_InvParts_C_%d"), clickcnt - 2);
            FString ExpectedButtonName1 = FString::Printf(TEXT("WBP_InvParts_C_%d"), clickcnt - 1);

            TSoftObjectPtr<USkeletalMesh> MeshAsset;

            if (clickcnt == 0)
            {
                USkeletalMesh* MeshAsset1 = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/XR_HSB/Character/Player_Pants_Short.Player_Pants_Short"));
                FString AssetPath = TEXT("/Game/XR_HSB/Character/Player_Pants_Short.Player_Pants_Short");
                FSoftObjectPath SoftObjectPath(AssetPath);
                UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftObjectPath, [this, SoftObjectPath]()
                {
                    USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(SoftObjectPath.TryLoad());
                    if (LoadedMesh)
                    {
                        Character->LowerBodyMeshComp->SetSkeletalMesh(LoadedMesh);
                        UE_LOG(LogTemp, Warning, TEXT("Success to load Mesh dynamically."));
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("Failed to load SkeletalMesh dynamically."));
                    }
                });
            }
            else if (clickcnt == 1)
            {
                USkeletalMesh* MeshAsset1 = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/XR_HSB/Character/Player_Pants_Short.Player_Pants_Short"));
                FString AssetPath = TEXT("/Game/XR_HSB/Character/Player_Pants_Short.Player_Pants_Short");
                FSoftObjectPath SoftObjectPath(AssetPath);
                UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftObjectPath, [this, SoftObjectPath]()
                {
                    USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(SoftObjectPath.TryLoad());
                    if (LoadedMesh)
                    {
                        Character->LowerBodyMeshComp->SetSkeletalMesh(nullptr);
                        UE_LOG(LogTemp, Warning, TEXT("Success to load Mesh dynamically."));
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("Failed to load SkeletalMesh dynamically."));
                    }
                });
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        else if ( Category == "Shoes" )
        {
            // 해당 그림에 맞는 Skeletal mesh를 찾는다

            TargetMeshComponent = Character->FeetMeshComp; // 캐릭터의 Skeletal Mesh 참조

            FString ExpectedButtonName0 = FString::Printf(TEXT("WBP_InvParts_C_%d"), clickcnt - 2);
            FString ExpectedButtonName1 = FString::Printf(TEXT("WBP_InvParts_C_%d"), clickcnt - 1);

            TSoftObjectPtr<USkeletalMesh> MeshAsset;

            if (clickcnt == 0)
            {
                USkeletalMesh* MeshAsset1 = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/XR_HSB/Character/Player_Shoes_v1.Player_Shoes_v1"));
                FString AssetPath = TEXT("/Game/XR_HSB/Character/Player_Shoes_v1.Player_Shoes_v1");
                FSoftObjectPath SoftObjectPath(AssetPath);
                UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftObjectPath, [this, SoftObjectPath]()
                {
                    USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(SoftObjectPath.TryLoad());
                    if (LoadedMesh)
                    {
                        Character->FeetMeshComp->SetSkeletalMesh(LoadedMesh);
                        UE_LOG(LogTemp, Warning, TEXT("Success to load Mesh dynamically."));
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("Failed to load SkeletalMesh dynamically."));
                    }
                });
            }
            else if (clickcnt == 1)
            {
                USkeletalMesh* MeshAsset1 = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/XR_HSB/Character/Player_Shoes_v1.Player_Shoes_v1"));
                FString AssetPath = TEXT("/Game/XR_HSB/Character/Player_Shoes_v1.Player_Shoes_v1");
                FSoftObjectPath SoftObjectPath(AssetPath);
                UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftObjectPath, [this, SoftObjectPath]()
                {
                    USkeletalMesh* LoadedMesh = Cast<USkeletalMesh>(SoftObjectPath.TryLoad());
                    if (LoadedMesh)
                    {
                        Character->FeetMeshComp->SetSkeletalMesh(nullptr);
                        UE_LOG(LogTemp, Warning, TEXT("Success to load Mesh dynamically."));
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("Failed to load SkeletalMesh dynamically."));
                    }
                });
            }
        }
	}
}