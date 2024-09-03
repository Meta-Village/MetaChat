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

void UInvSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 버튼 클릭 이벤트 바인딩
    if ( Button_ItemIcon_1 )
    {
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

//             // Overlay 위젯을 동적으로 생성하고, 이미지와 버튼 추가
//             OverlayWidget = NewObject<UOverlay>(this);
//             if ( OverlayWidget )
//             {
//                 // Overlay에 이미지 추가
//                 UOverlaySlot* ImageSlot = OverlayWidget->AddChildToOverlay(ImageWidget);
//                 if ( ImageSlot )
//                 {
//                     ImageSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
//                     ImageSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
//                 }
// 
//                 // 아이콘 이미지 업데이트
//                 ImageWidget->SetBrushFromTexture(ItemsData[i].ItemIcon);
// 
//                 // 버튼 위젯 동적 생성
//                 UButton* ButtonWidget = NewObject<UButton>(this , FName(*FString::Printf(TEXT("Button_ItemIcon_%d") , i + 1)));
//                 if ( ButtonWidget )
//                 {
//                     // 버튼 클릭 이벤트 바인딩
//                     ButtonWidget->OnClicked.AddDynamic(this , &UInvSlotWidget::OnItemClicked);
// 
//                     // Overlay에 버튼 추가
//                     UOverlaySlot* ButtonSlot = OverlayWidget->AddChildToOverlay(ButtonWidget);
//                     if ( ButtonSlot )
//                     {
//                         ButtonSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
//                         ButtonSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
//                     }
//                 }
//            }
        }
    }

	if(ItemsData.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetItemData called with empty ItemsData."));
	}
}

void UInvSlotWidget::OnItemClicked()
{
    // 카테고리를 구분한다
    Character = CastChecked<ACustomCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld() , 0));
    // Mesh 설정할 캐릭터의 SkeletalMeshComponent
    USkeletalMeshComponent* TargetMeshComponent = nullptr;
    if(Character)
	{
        if(Category == "" )
            UE_LOG(LogTemp , Warning , TEXT("Problem in Loading Category."));

        // 버튼 이름 가져오기
        FString ButtonName = GetName();
        ////////////////////////////////////////////////////////////////////////////////////////////
        if ( Category == "Hair" )
        {
            // 해당 그림에 맞는 Skeletal mesh를 찾는다

            TargetMeshComponent = Character->HairMesh; // 캐릭터의 Skeletal Mesh 참조
            if ( ButtonName.Contains("WBP_InvParts_C_0") )
            {
                USkeletalMesh* NewMesh = LoadObject<USkeletalMesh>(nullptr , TEXT("/Script/Engine.SkeletalMesh'/Game/XR_HSB/Character/Player_test_hat.Player_test_hat'"));
                if ( NewMesh == nullptr )
                {
                    UE_LOG(LogTemp , Error , TEXT("Failed to load SkeletalMesh 1"));
                }
                if ( TargetMeshComponent )
                {
                    TargetMeshComponent->SetSkeletalMesh(NewMesh);
                    TargetMeshComponent->MarkRenderStateDirty();

                    UE_LOG(LogTemp , Warning , TEXT("Success to load Mesh 1"));
                }
                else
                {
                    UE_LOG(LogTemp , Error , TEXT("Failed to Set SkeletalMesh 1"));
                }
            }
            else if ( ButtonName.Contains("WBP_InvParts_C_1") )
            {
                USkeletalMesh* NewMesh = LoadObject<USkeletalMesh>(nullptr , TEXT("/Script/Engine.SkeletalMesh'/Game/XR_HSB/Character/Player_test_hat.Player_test_hat'"));
                if ( NewMesh == nullptr )
                {
                    UE_LOG(LogTemp , Error , TEXT("Failed to load SkeletalMesh 2"));
                }
                if ( TargetMeshComponent )
                {
                    TargetMeshComponent->SetSkeletalMesh(NewMesh);
                    UE_LOG(LogTemp , Warning , TEXT("Success to load Mesh 2"));
                }
                else
                {
                    UE_LOG(LogTemp , Error , TEXT("Failed to Set SkeletalMesh 2"));
                }
            }
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        else if ( Category == "Upper" )
        {
            // 해당 그림에 맞는 Skeletal mesh를 찾는다

            TargetMeshComponent = Character->UpperBodyMesh; // 캐릭터의 Skeletal Mesh 참조
            if ( ButtonName.Contains("WBP_InvParts_C_0") )
            {
                USkeletalMesh* NewMesh = LoadObject<USkeletalMesh>(nullptr , TEXT("/Script/Engine.SkeletalMesh'/Game/XR_HSB/Character/Player_test_cloth.Player_test_cloth'"));
                if ( NewMesh == nullptr )
                {
                    UE_LOG(LogTemp , Error , TEXT("Failed to load SkeletalMesh 1"));
                }
                if ( TargetMeshComponent )
                {
                    TargetMeshComponent->SetSkeletalMesh(NewMesh);
                    TargetMeshComponent->MarkRenderStateDirty();

                    UE_LOG(LogTemp , Warning , TEXT("Success to load Mesh 1"));
                }
                else
                {
                    UE_LOG(LogTemp , Error , TEXT("Failed to Set SkeletalMesh 1"));
                }
            }
            else if ( ButtonName.Contains("WBP_InvParts_C_1") )
            {
                USkeletalMesh* NewMesh = LoadObject<USkeletalMesh>(nullptr , TEXT("/Script/Engine.SkeletalMesh'/Game/XR_HSB/Character/Player_test_cloth.Player_test_cloth'"));
                if ( NewMesh == nullptr )
                {
                    UE_LOG(LogTemp , Error , TEXT("Failed to load SkeletalMesh 2"));
                }
                if ( TargetMeshComponent )
                {
                    TargetMeshComponent->SetSkeletalMesh(NewMesh);
                    UE_LOG(LogTemp , Warning , TEXT("Success to load Mesh 2"));
                }
                else
                {
                    UE_LOG(LogTemp , Error , TEXT("Failed to Set SkeletalMesh 2"));
                }
            }
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        else if ( Category == "Lower" )
        {
            // 해당 그림에 맞는 Skeletal mesh를 찾는다

            TargetMeshComponent = Character->LowerBodyMesh; // 캐릭터의 Skeletal Mesh 참조
            if ( ButtonName.Contains("WBP_InvParts_C_0") )
            {
                USkeletalMesh* NewMesh = LoadObject<USkeletalMesh>(nullptr , TEXT("/Script/Engine.SkeletalMesh'/Game/XR_HSB/Character/Player_test_pants.Player_test_pants'"));
                if ( NewMesh == nullptr )
                {
                    UE_LOG(LogTemp , Error , TEXT("Failed to load SkeletalMesh 1"));
                }
                if ( TargetMeshComponent )
                {
                    TargetMeshComponent->SetSkeletalMesh(NewMesh);
                    TargetMeshComponent->MarkRenderStateDirty();

                    UE_LOG(LogTemp , Warning , TEXT("Success to load Mesh 1"));
                }
                else
                {
                    UE_LOG(LogTemp , Error , TEXT("Failed to Set SkeletalMesh 1"));
                }
            }
            else if ( ButtonName.Contains("WBP_InvParts_C_1") )
            {
                USkeletalMesh* NewMesh = LoadObject<USkeletalMesh>(nullptr , TEXT("/Script/Engine.SkeletalMesh'/Game/XR_HSB/Character/Player_test_pants.Player_test_pants'"));
                if ( NewMesh == nullptr )
                {
                    UE_LOG(LogTemp , Error , TEXT("Failed to load SkeletalMesh 2"));
                }
                if ( TargetMeshComponent )
                {
                    TargetMeshComponent->SetSkeletalMesh(NewMesh);
                    UE_LOG(LogTemp , Warning , TEXT("Success to load Mesh 2"));
                }
                else
                {
                    UE_LOG(LogTemp , Error , TEXT("Failed to Set SkeletalMesh 2"));
                }
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////
        else if ( Category == "Shoes" )
        {
            // 해당 그림에 맞는 Skeletal mesh를 찾는다

            TargetMeshComponent = Character->FeetMesh; // 캐릭터의 Skeletal Mesh 참조
            if ( ButtonName.Contains("WBP_InvParts_C_0") )
            {
                USkeletalMesh* NewMesh = LoadObject<USkeletalMesh>(nullptr , TEXT("/Script/Engine.SkeletalMesh'/Game/XR_HSB/Character/Player_test_shoes.Player_test_shoes'"));
                if ( NewMesh == nullptr )
                {
                    UE_LOG(LogTemp , Error , TEXT("Failed to load SkeletalMesh 1"));
                }
                if ( TargetMeshComponent )
                {
                    TargetMeshComponent->SetSkeletalMesh(NewMesh);
                    TargetMeshComponent->MarkRenderStateDirty();

                    UE_LOG(LogTemp , Warning , TEXT("Success to load Mesh 1"));
                }
                else
                {
                    UE_LOG(LogTemp , Error , TEXT("Failed to Set SkeletalMesh 1"));
                }
            }
            else if ( ButtonName.Contains("WBP_InvParts_C_1") )
            {
                USkeletalMesh* NewMesh = LoadObject<USkeletalMesh>(nullptr , TEXT("/Script/Engine.SkeletalMesh'/Game/XR_HSB/Character/Player_test_shoes.Player_test_shoes'"));
                if ( NewMesh == nullptr )
                {
                    UE_LOG(LogTemp , Error , TEXT("Failed to load SkeletalMesh 2"));
                }
                if ( TargetMeshComponent )
                {
                    TargetMeshComponent->SetSkeletalMesh(NewMesh);
                    UE_LOG(LogTemp , Warning , TEXT("Success to load Mesh 2"));
                }
                else
                {
                    UE_LOG(LogTemp , Error , TEXT("Failed to Set SkeletalMesh 2"));
                }
            }
        }
	}
}