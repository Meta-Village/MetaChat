#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "InvSlotWidget.generated.h"

USTRUCT(BlueprintType)
struct FSlot : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Category;
};

UCLASS()
class METACHAT_API UInvSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:	
	virtual void NativeConstruct() override;

	UPROPERTY()
	class UOverlay* OverlayWidget;

	// 이미지 바인딩
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_ItemIcon_1;


	UFUNCTION(BluePrintCallable)
	void SetItemData(const TArray<FSlot>& ItemsData);

	// 아이템 클릭하면 캐릭터 파트에 적용
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_ItemIcon_1;

	UFUNCTION(BlueprintCallable)
	void OnItemClicked();

	int32 clickcnt=0;

	UPROPERTY()
	class ACustomCharacter* Character;

	// 카테고리 변수
	FName Category;
};
