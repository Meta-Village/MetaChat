﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonParseLib.generated.h"

/**
 * 
 */
UCLASS()
class METACHAT_API UJsonParseLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static FString JsonParse(const FString& json);
	static FString JsonParsePassword(const FString& json);
	static void JsonParsePassword(const FString& json, int32& WorldID, FString WorldName);
	static bool JsonParseUserInfo(const FString& json, FString& UserToken);
	static FString MakeJson(const TMap<FString, FString> source);
};
