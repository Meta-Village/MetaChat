#include "HSB/InvSlotWidget.h"
#include "Engine/Texture2D.h"
#include "Internationalization/Text.h"
#include "Components/Image.h"

void UInvSlotWidget::SetItemData(const TArray<FSlot>& ItemsData)
{
	// UI 컴포넌트에 데이터 반영
    for ( int32 i = 0; i < ItemsData.Num(); ++i )
    {
        // 동적 위젯 이름 생성
        FString WidgetName = FString::Printf(TEXT("Image_ItemIcon_%d") , i + 1);
        UImage* ImageWidget = Cast<UImage>(GetWidgetFromName(FName(*WidgetName)));

        if ( ImageWidget )
        {
            // 아이콘 이미지 업데이트
            ImageWidget->SetBrushFromTexture(ItemsData[i].ItemIcon);
        }
    }

	if(ItemsData.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetItemData called with empty ItemsData."));
	}
}
