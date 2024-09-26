#include "HSB/CustomWidget.h"
#include "HSB/InvSlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/Image.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/SkeletalMeshComponent.h"
#include "Delegates/Delegate.h"
#include "HSB/CustomCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "SB/CustomSaveGame.h"

void UCustomWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 버튼 클릭 이벤트 바인딩
    if ( BtnHair )
    {
        BtnHair->OnClicked.AddDynamic(this , &UCustomWidget::OnHairButtonClicked);
    }
    // 버튼 클릭 이벤트 바인딩
    if ( BtnUpper )
    {
        BtnUpper->OnClicked.AddDynamic(this , &UCustomWidget::OnUpperButtonClicked);
    }
    // 버튼 클릭 이벤트 바인딩
    if ( BtnLower )
    {
        BtnLower->OnClicked.AddDynamic(this , &UCustomWidget::OnLowerButtonClicked);
    }
    // 버튼 클릭 이벤트 바인딩
    if ( BtnShoes )
    {
        BtnShoes->OnClicked.AddDynamic(this , &UCustomWidget::OnShoesButtonClicked);
    }
    // 버튼 클릭 이벤트 바인딩
    if ( BtnApply )
    {
        BtnApply->OnClicked.AddDynamic(this , &UCustomWidget::OnButtonApply);
    }
    // 뒤로가기 버튼 이벤트 바인딩
    if (BtnApply)
    {
        BtnBack->OnClicked.AddDynamic(this, &UCustomWidget::OnButtonBack);
    }
//     // 버튼 클릭 이벤트 바인딩
//     if ( BtnUndo )
//     {
//         BtnUndo->OnClicked.AddDynamic(this , &UCustomWidget::OnButtonUndo);
//     }

    // 캐릭터 변수로 저장해둔 P_ClickLoad값 가져오기
    ACustomCharacter* Character = CastChecked<ACustomCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (Character)
    {
        P_clickcnt = Character->P_ClickLoad;
    }
}

void UCustomWidget::InitSlot()
{
    // DataTable 불러오기
    UDataTable* DataTable = LoadObject<UDataTable>(nullptr, TEXT("/Script/Engine.DataTable'/Game/XR_HSB/BluePrints/DT_InvInfo.DT_InvInfo'"));
    if (!DataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("DataTable failed to load."));
        return;
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("DataTable loaded successfully."));
    }

    // 원하는 카테고리 아이템 필터링
    TArray<FSlot*> FilteredItems;
    DataTable->GetAllRows(TEXT("GENERAL"), FilteredItems);// 'TEXT("GENERAL")은 문자열 리터럴을 유니코드로 변환하는 매크로

    UE_LOG(LogTemp, Log, TEXT("FilteredItems count: %d"), FilteredItems.Num());

    // 기존 슬롯 제거
    PartsPanel->ClearChildren();

    int32 Row = 0;
    int32 Column = 0;
    int32 num = 0;

    // 아이템 데이터 바탕으로 슬롯 생성 및 추가
    for(const FSlot* fSlot : FilteredItems)
	{
        // 카테고리 필터링
        if(fSlot && fSlot->Category == DesiredCategory)
		{
            InvSlot = CastChecked<UInvSlotWidget>(CreateWidget(GetWorld(), InvFactory));
            if (InvSlot)
            {
                // 슬롯 가시성 및 레이아웃 확인
                InvSlot->SetVisibility(ESlateVisibility::Visible);

                // fSlot을 배열로 만들어서 전달
                TArray<FSlot> SlotArray;
                SlotArray.Add(*fSlot);

                InvSlot->clickcnt = num++;
                InvSlot->SetItemData(SlotArray);
                // `InvSlot`의 `Category` 값 설정
                InvSlot->Category = DesiredCategory;

                // Grid에 슬롯 추가
                PartsPanel->AddChildToUniformGrid(InvSlot, Row, Column);

                // 컬럼 값 증가
                Column++;

                // 2열 그리드를 기준으로 행과 열을 자동으로 업데이트
                if (Column >= 2) // 예를 들어 2열씩 배치하고 싶다면
                {
                    Column = 0;
                    Row++;
                }

                if (!PartsPanel)
                {
                    UE_LOG(LogTemp, Error, TEXT("PartsPanel is not valid."));
                    return;
                }

                //InvSlot->clickcnt = P_clickcnt; // 클릭 값 전달 (계속 InvSlot 갱신돼서 clickcnt값 업데이트 안 되는 문제 때문)
            }
		}
	}
}

void UCustomWidget::OnHairButtonClicked()
{
    P_clickcnt += 2;
    UE_LOG(LogTemp, Log, TEXT("%d"), P_clickcnt);

    DesiredCategory = "Hair";
    InitSlot(); // 슬롯 재생성

    // 버튼 클릭 시 이미지가 변경되도록 설정
    if (Img_hair && NewHairTexture)
    {
        // UImage의 Brush 속성 변경
        Img_hair->SetBrushFromTexture(NewHairTexture, true);
        Img_upper->SetBrushFromTexture(OriginUpperTexture, true);
        Img_lower->SetBrushFromTexture(OriginLowerTexture, true);
        Img_shoes->SetBrushFromTexture(OriginShoesTexture, true);
    }

    UE_LOG(LogTemp , Warning , TEXT("%s"), *DesiredCategory.ToString());
}

void UCustomWidget::OnUpperButtonClicked()
{
    P_clickcnt += 2;
    UE_LOG(LogTemp, Log, TEXT("%d"), P_clickcnt);

    DesiredCategory = "Upper";
    InitSlot(); // 슬롯 재생성

    // 버튼 클릭 시 이미지가 변경되도록 설정
    if (Img_upper && NewUpperTexture)
    {
        // UImage의 Brush 속성 변경
        Img_hair->SetBrushFromTexture(OriginHairTexture, true);
        Img_upper->SetBrushFromTexture(NewUpperTexture, true);
        Img_lower->SetBrushFromTexture(OriginLowerTexture, true);
        Img_shoes->SetBrushFromTexture(OriginShoesTexture, true);
    }

    UE_LOG(LogTemp , Warning , TEXT("%s") , *DesiredCategory.ToString());
}

void UCustomWidget::OnLowerButtonClicked()
{
    P_clickcnt += 2;
    UE_LOG(LogTemp, Log, TEXT("%d"), P_clickcnt);

    DesiredCategory = "Lower";
    InitSlot(); // 슬롯 재생성

    // 버튼 클릭 시 이미지가 변경되도록 설정
    if (Img_lower && NewLowerTexture)
    {
        // UImage의 Brush 속성 변경
        Img_hair->SetBrushFromTexture(OriginHairTexture, true);
        Img_upper->SetBrushFromTexture(OriginUpperTexture, true);
        Img_lower->SetBrushFromTexture(NewLowerTexture, true);
        Img_shoes->SetBrushFromTexture(OriginShoesTexture, true);
    }

    UE_LOG(LogTemp , Warning , TEXT("%s") , *DesiredCategory.ToString());
}

void UCustomWidget::OnShoesButtonClicked()
{
    P_clickcnt += 2;
    UE_LOG(LogTemp, Log, TEXT("%d"), P_clickcnt);

    DesiredCategory = "Shoes";
    InitSlot(); // 슬롯 재생성

    // 버튼 클릭 시 이미지가 변경되도록 설정
    if (Img_shoes && NewShoesTexture)
    {
        // UImage의 Brush 속성 변경
        Img_hair->SetBrushFromTexture(OriginHairTexture, true);
        Img_upper->SetBrushFromTexture(OriginUpperTexture, true);
        Img_lower->SetBrushFromTexture(OriginLowerTexture, true);
        Img_shoes->SetBrushFromTexture(NewShoesTexture, true);
    }

    UE_LOG(LogTemp , Warning , TEXT("%s") , *DesiredCategory.ToString());
}

void UCustomWidget::OnButtonApply()
{
    UE_LOG(LogTemp , Warning , TEXT("Apply Button Click"));

    ACustomCharacter* Character = CastChecked<ACustomCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if ( !Character )
    {
        UE_LOG(LogTemp , Error , TEXT("Failed to get player character"));
        return;
    }

    // 현재 메쉬 상태 저장
    UCustomSaveGame* SaveGameInstance = CastChecked<UCustomSaveGame>(UGameplayStatics::CreateSaveGameObject(UCustomSaveGame::StaticClass()));
    if(SaveGameInstance )
	{
		if ( Character->HairMeshComp )
		{
			FString MeshPath = Character->HairMeshComp->SkeletalMesh->GetPathName();
			SaveGameInstance->SavedMeshes.Add("Hair" , MeshPath);
		}
		if ( Character->UpperBodyMeshComp )
		{
			FString MeshPath = Character->UpperBodyMeshComp->SkeletalMesh->GetPathName();
			SaveGameInstance->SavedMeshes.Add("Upper" , MeshPath);
		}
		if ( Character->LowerBodyMeshComp )
		{
			FString MeshPath = Character->LowerBodyMeshComp->SkeletalMesh->GetPathName();
			SaveGameInstance->SavedMeshes.Add("Lower" , MeshPath);
		}
		if ( Character->FeetMeshComp )
		{
			FString MeshPath = Character->FeetMeshComp->SkeletalMesh->GetPathName();
			SaveGameInstance->SavedMeshes.Add("Shoes" , MeshPath);
		}

		// SaveGame 저장
		UGameplayStatics::SaveGameToSlot(SaveGameInstance , TEXT("CharacterMeshSaveSlot") , 0);
		UE_LOG(LogTemp , Warning , TEXT("Mesh states have been saved."));


	}
}

void UCustomWidget::OnButtonBack()
{
    ACustomCharacter* Character = CastChecked<ACustomCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if(Character)
    {
        Character->P_ClickLoad = P_clickcnt;
    }
}

// void UCustomWidget::OnButtonUndo()
// {
//     UE_LOG(LogTemp , Warning , TEXT("Undo Button Click"));
// 
//     ACustomCharacter* Character = CastChecked<ACustomCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld() , 0));
//     if ( !Character )
//     {
//         UE_LOG(LogTemp , Error , TEXT("Failed to get player character"));
//         return;
//     }
// 
//     // SaveGame 로드
//     UCustomSaveGame* SaveGameInstance = CastChecked<UCustomSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("CharacterMeshSaveSlot"), 0));
//     if ( SaveGameInstance )
//     {
//         // Mesh들 복원
//         FString* MeshPath_h = SaveGameInstance->SavedMeshes.Find("Hair");
//         if ( MeshPath_h )
//         {
//             USkeletalMesh* PreviousMesh = LoadObject<USkeletalMesh>(nullptr, 
//             **MeshPath_h);
//             if ( PreviousMesh )
//             {
//                 Character->HairMesh->SetSkeletalMesh(PreviousMesh);
//                 UE_LOG(LogTemp , Warning , TEXT("Hair mesh has been restored."));
//             }
//         }
//         FString* MeshPath_u = SaveGameInstance->SavedMeshes.Find("Upper");
//         if ( MeshPath_u )
//         {
//             USkeletalMesh* PreviousMesh = LoadObject<USkeletalMesh>(nullptr ,
//             **MeshPath_u);
//             if ( PreviousMesh )
//             {
//                 Character->UpperBodyMesh->SetSkeletalMesh(PreviousMesh);
//                 UE_LOG(LogTemp , Warning , TEXT("Upper mesh has been restored."));
//             }
//         }
//         FString* MeshPath_l = SaveGameInstance->SavedMeshes.Find("Lower");
//         if ( MeshPath_l )
//         {
//             USkeletalMesh* PreviousMesh = LoadObject<USkeletalMesh>(nullptr ,
//             **MeshPath_l);
//             if ( PreviousMesh )
//             {
//                 Character->LowerBodyMesh->SetSkeletalMesh(PreviousMesh);
//                 UE_LOG(LogTemp , Warning , TEXT("Lower mesh has been restored."));
//             }
//         }
//         FString* MeshPath_f = SaveGameInstance->SavedMeshes.Find("Feet");
//         if ( MeshPath_f )
//         {
//             USkeletalMesh* PreviousMesh = LoadObject<USkeletalMesh>(nullptr ,
//             **MeshPath_f);
//             if ( PreviousMesh )
//             {
//                 Character->FeetMesh->SetSkeletalMesh(PreviousMesh);
//                 UE_LOG(LogTemp , Warning , TEXT("Feet mesh has been restored."));
//             }
//         }
//     }
//     else
//     {
//         UE_LOG(LogTemp , Error , TEXT("Failed to load save game"));
//     }
// }
