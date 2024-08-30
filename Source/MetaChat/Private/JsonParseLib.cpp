// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonParseLib.h"
#include "JsonObjectConverter.h"

FString UJsonParseLib::JsonParse(const FString& json)
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
FString UJsonParseLib::JsonParsePassword(const FString& json)
{
	//리더기를 만들고
	TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(json);

	//파싱 결과를 담을 변수 선언
	TSharedPtr<FJsonObject> result = MakeShareable(new FJsonObject());
	//해석을 한다.
	FString returnValue;
	returnValue.Append(FString::Printf(TEXT("result %s\n"), *json));
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

FString UJsonParseLib::MakeJson(const TMap<FString, FString> source)
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
