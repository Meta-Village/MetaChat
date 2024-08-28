#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomWidget.generated.h"


UCLASS()
class METACHAT_API UCustomWidget  : public UUserWidget
{
	GENERATED_BODY()
	
	UFUNCTION()
	void OnItemClicked(UObject* Item);
};
