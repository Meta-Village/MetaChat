// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScreenActor.generated.h"

UCLASS()
class METACHAT_API AScreenActor : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AScreenActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void UpdateTexture();
	UTexture2D* CaptureScreenToTexture();
	UTexture2D* CapturedTexture;
	class UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* sceneComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* WindowScreenPlaneMesh;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> MainWidgetFactory;
	class ULSJMainWidget* MainWidget;

	//블루프린트 함수 호출
	void StopLookSharingScreen();
	void BeginLookSharingScreen();
};
