// Fill out your copyright notice in the Description page of Project Settings.


#include "HSB/CustomAnimInstance.h"
#include "HSB/CustomCharacter.h"
#include "Animation/AnimMontage.h"

void UCustomAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ACustomCharacter* character = Cast<ACustomCharacter>(TryGetPawnOwner());
	if (nullptr == character)
	{
		return;
	}
}

void UCustomAnimInstance::PlayIdleMontage()
{
	Montage_Play(Idle);
}

void UCustomAnimInstance::PlayWalkMontage()
{
	Montage_Play(Walk);
}

void UCustomAnimInstance::PlaySitMontage()
{
	Montage_Play(Sit);
}
