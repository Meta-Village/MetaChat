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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class UUserWidget> InvFactory;

	UPROPERTY()
	class UInvSlotWidget* InvSlot;

	// 카테고리
	FName DesiredCategory;

	// 초기화
	UFUNCTION()
	void InitSlot();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) 
	int32 P_clickcnt;

	// 뒤로가기 버튼 바인딩 (블프에 추가 로직 있음)
	UPROPERTY(meta=(BindWidget))
	class UButton* BtnBack;

	UFUNCTION()
	void OnButtonBack();

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
	UPROPERTY(EditAnywhere, Category = "UI")
	class UTexture2D* OriginHairTexture;

	UPROPERTY(meta = (BindWidget))
	class UImage* Img_upper;
	// 변경될 이미지
	UPROPERTY(EditAnywhere, Category = "UI")
	class UTexture2D* NewUpperTexture;
	UPROPERTY(EditAnywhere, Category = "UI")
	class UTexture2D* OriginUpperTexture;

	UPROPERTY(meta = (BindWidget))
	class UImage* Img_lower;
	// 변경될 이미지
	UPROPERTY(EditAnywhere, Category = "UI")
	class UTexture2D* NewLowerTexture;
	UPROPERTY(EditAnywhere, Category = "UI")
	class UTexture2D* OriginLowerTexture;

	UPROPERTY(meta = (BindWidget))
	class UImage* Img_shoes;
	// 변경될 이미지
	UPROPERTY(EditAnywhere, Category = "UI")
	class UTexture2D* NewShoesTexture;
	UPROPERTY(EditAnywhere, Category = "UI")
	class UTexture2D* OriginShoesTexture;

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
