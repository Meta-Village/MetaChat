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
	static void JsonParsePassword(const FString& json, TArray<FString>& ArrayUserID, TArray<FString>& ArrayChatContent);
	static FString MakeJson(const TMap<FString, FString> source);
};
