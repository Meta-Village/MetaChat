﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CustomAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class METACHAT_API UCustomAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* Idle;
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* Walk;
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* Sit;

	UFUNCTION(BlueprintCallable)
	void PlayIdleMontage();
	UFUNCTION(BlueprintCallable)
	void PlayWalkMontage();
	UFUNCTION(BlueprintCallable)
	void PlaySitMontage();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool IsWalking;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool IsSitting;
};
