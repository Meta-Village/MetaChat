// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputAction.h"
#include "HSB/CustomCharacter.h"
#include "MetaChatPlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class AMetaChatPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMetaChatPlayerController();

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationClickAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationTouchAction;
//---------------------------------------------------------------------
	// 레벨 전환 처리
	void RequestTravelToCustomizationLevel();
// 	UFUNCTION(BlueprintCallable)
// 	void OnCustomButtonClicked();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRequestTravelToCustomLevel();
	void ServerRequestTravelToCustomLevel_Implementation();
	bool ServerRequestTravelToCustomLevel_Validate();

	// 커스텀 데이터를 캐릭터에 전달하는 중간 역할
	UFUNCTION(BlueprintCallable, Category="Customization")
	void SubmitcustomizationData(const FCharacterCustomizationData& NewData);
//---------------------------------------------------------------------

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	virtual void SetupInputComponent() override;
	
	// To add mapping context
	virtual void BeginPlay();

	/** Input handlers for SetDestination action. */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();

	void OnTouchTriggered();
	void OnTouchReleased();

private:
	FVector CachedDestination;

	bool bIsTouch; // Is it a touch device
	float FollowTime; // For how long it has been pressed

	void MoveToLocation(const FVector& Location);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerMoveToLocation(const FVector& Location);
	void ServerMoveToLocation_Implementation(const FVector& Location);
	bool ServerMoveToLocation_Validate(const FVector& Location);

	void MoveToLocationTick();
	FTimerHandle MoveTimerHandle;
};


