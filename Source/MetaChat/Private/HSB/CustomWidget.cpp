#include "HSB/CustomWidget.h"
#include "HSB/InvSlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/Image.h"
#include "Blueprint/UserWidget.h"
#include "Components/SkeletalMeshComponent.h"

void UCustomWidget::InitSlot()
{
    // DataTable 불러오기
    UDataTable* DataTable = LoadObject<UDataTable>(nullptr, TEXT("/Script/Engine.DataTable'/Game/HSB/BluePrints/DT_InvInfo.DT_InvInfo'"));
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

                InvSlot->SetItemData(SlotArray);

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
            }
			
		}
	}


}

void UCustomWidget::OnCategoryButtonClicked(FName Category)
{
    DesiredCategory = Category;
    InitSlot(); // 슬롯 재생성
}


