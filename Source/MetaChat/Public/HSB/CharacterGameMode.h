#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "InvSlotWidget.h"
#include "CharacterGameMode.generated.h"


UCLASS()
class METACHAT_API ACharacterGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	TArray<FInventoryItem> InventoryItems;

};
