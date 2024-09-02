#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomWidget.generated.h"


UCLASS()
class METACHAT_API UCustomWidget  : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	// Panel
	UPROPERTY(meta=(BindWidget))
	class UUniformGridPanel* PartsPanel;
	
	// TSubClass
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> InvFactory;

	UPROPERTY()
	class UInvSlotWidget* InvSlot;

	// 카테고리
	FName DesiredCategory;

	// 초기화
	UFUNCTION()
	void InitSlot();

	// 카테고리 클릭하면 슬롯 필터링
	UPROPERTY(meta=(BindWidget))
	class UButton* BtnHair;
	UPROPERTY(meta=(BindWidget))
	class UButton* BtnUpper;
	UPROPERTY(meta=(BindWidget))
	class UButton* BtnLower;
	UPROPERTY(meta=(BindWidget))
	class UButton* BtnShoes;

	// 적용 버튼 바인딩
	UPROPERTY(meta=(BindWidget))
	class UButton* BtnApply;


	UFUNCTION()
	void OnHairButtonClicked();
	UFUNCTION()
	void OnUpperButtonClicked();
	UFUNCTION()
	void OnLowerButtonClicked();
	UFUNCTION()
	void OnShoesButtonClicked();

	UFUNCTION()
	void OnButtonApply(); // 저장된 내용은 SaveGame에서 load함수로 불러올 수 있음
};
