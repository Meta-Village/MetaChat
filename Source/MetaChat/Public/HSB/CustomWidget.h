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

	int32 P_clickcnt = 0;

	// 카테고리 클릭하면 슬롯 필터링
	UPROPERTY(meta=(BindWidget))
	class UButton* BtnHair;
	UPROPERTY(meta=(BindWidget))
	class UButton* BtnUpper;
	UPROPERTY(meta=(BindWidget))
	class UButton* BtnLower;
	UPROPERTY(meta=(BindWidget))
	class UButton* BtnShoes;

	// 버튼 별 이미지 바인딩
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_hair;
	// 변경될 이미지
	UPROPERTY(EditAnywhere, Category = "UI")
	class UTexture2D* NewHairTexture;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_upper;
	// 변경될 이미지
	UPROPERTY(EditAnywhere, Category = "UI")
	class UTexture2D* NewUpperTexture;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_lower;
	// 변경될 이미지
	UPROPERTY(EditAnywhere, Category = "UI")
	class UTexture2D* NewLowerTexture;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_shoes;
	// 변경될 이미지
	UPROPERTY(EditAnywhere, Category = "UI")
	class UTexture2D* NewShoesTexture;

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
