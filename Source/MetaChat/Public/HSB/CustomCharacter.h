// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CustomCharacter.generated.h"

UCLASS()
class METACHAT_API ACustomCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACustomCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Lower Body
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* LowerBodyMesh;


    // Upper Body
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite , Category = "Character Parts")
    USkeletalMeshComponent* UpperBodyMesh;

    // Head
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite , Category = "Character Parts")
    USkeletalMeshComponent* HeadMesh;

    // Hair
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite , Category = "Character Parts")
    USkeletalMeshComponent* HairMesh;

    // Feet
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite , Category = "Character Parts")
    USkeletalMeshComponent* FeetMesh;
};
