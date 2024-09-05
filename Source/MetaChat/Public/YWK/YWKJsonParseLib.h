// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "YWKJsonParseLib.generated.h"

/**
 * 
 */
UCLASS()
class METACHAT_API UYWKJsonParseLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static FString JsonParse(const FString& json);
	static FString JsonParsePassword(const FString& json);
	static FString MakeJson(const TMap<FString, FString> source);
};
