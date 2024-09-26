// Fill out your copyright notice in the Description page of Project Settings.


#include "YWK/YWKJsonParseLib.h"
#include "JsonObjectConverter.h"

FString UYWKJsonParseLib::JsonParse(const FString& json)
{
	//리더기를 만들고
	TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(json);

	//파싱 결과를 담을 변수 선언
	TSharedPtr<FJsonObject> result = MakeShareable(new FJsonObject());
	//해석을 한다.
	FString returnValue;
	if (FJsonSerializer::Deserialize(reader, result))
	{
		TArray<TSharedPtr<FJsonValue>> parseDataList = result->GetArrayField(TEXT("items"));
		for (TSharedPtr<FJsonValue> data : parseDataList)
		{
			//책과 저자 이름
			FString bookName = data->AsObject()->GetStringField("bk_nm");
			FString authorName = data->AsObject()->GetStringField("aut_nm");
			returnValue.Append(FString::Printf(TEXT("bookName %s / authorName %s\n"), *bookName, *authorName));
		}
	}
	//반환한다
	return returnValue;
}
void UYWKJsonParseLib::JsonParsePassword(const FString& json, TArray<FString>& ArrayUserID, TArray<FString>& ArrayChatContent)
{

	// Create a JSON object pointer
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(json);

	// Deserialize the JSON string into a JSON object
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		// Get the 'data' array from the JSON object
		const TArray<TSharedPtr<FJsonValue>>* DataArray;
		if (JsonObject->TryGetArrayField(TEXT("data"), DataArray))
		{
			// Loop through each entry in the 'data' array
			for (const TSharedPtr<FJsonValue>& Entry : *DataArray)
			{
				TSharedPtr<FJsonObject> ChatEntry = Entry->AsObject();
				if (ChatEntry.IsValid())
				{
					// Extract the userId and chatContent fields
					FString UserId = ChatEntry->GetStringField(TEXT("userId"));
					FString ChatContent = ChatEntry->GetStringField(TEXT("chatContent"));
					ArrayUserID.Add(UserId);
					ArrayChatContent.Add(ChatContent);
					// Print or handle the values
					UE_LOG(LogTemp, Warning, TEXT("UserID: %s, ChatContent: %s"), *UserId, *ChatContent);
				}
			}
		}
	}
}

FString UYWKJsonParseLib::MakeJson(const TMap<FString, FString> source)
{
	//source를 jsonObject형식으로 만든다.
	TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
	for (TPair<FString, FString> pair : source)
	{
		jsonObject->SetStringField(pair.Key, pair.Value);
	}

	//writer를 만들어서 jsonobject를 인코딩해서
	FString json;
	TSharedRef<TJsonWriter<TCHAR>> writer = TJsonWriterFactory<TCHAR>::Create(&json);
	FJsonSerializer::Serialize(jsonObject.ToSharedRef(), writer);
	//반환한다
	return json;
}
