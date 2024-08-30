#include "HSB/InvSlotWidget.h"
#include "Engine/Texture2D.h"
#include "Internationalization/Text.h"
#include "Components/Image.h"

void UInvSlotWidget::SetItemData(const TArray<FSlot>& ItemsData)
{
	// UI 컴포넌트에 데이터 반영
	for(int32 i=0; i<ItemsData.Num(); i++)
	{
		UImage* NewImage = NewObject<UImage>(this);
		if (NewImage)
		{
			NewImage->SetBrushFromTexture(ItemsData[i].ItemIcon);
			// 생성된 이미지를 UI에 추가
		}
	}

	if(ItemsData.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetItemData called with empty ItemsData."));
	}
}
