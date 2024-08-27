#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InvSlotWidget.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FText ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    UTexture2D* ItemIcon;
};

UCLASS()
class METACHAT_API UInvSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable)
	void SetItemData(const FInventoryItem& ItemData);

	UFUNCTION(BlueprintCallable)
	void PopulateInventory();
};
