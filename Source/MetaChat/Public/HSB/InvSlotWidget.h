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
	// 이미지 바인딩
	UPROPERTY(meta=(BindWidget))
	class UImage* Image_ItemIcon_1;


	UFUNCTION(BluePrintCallable)
	void SetItemData(const TArray<FSlot>& ItemsData);

};
