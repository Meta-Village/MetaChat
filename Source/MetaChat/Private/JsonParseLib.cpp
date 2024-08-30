// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonParseLib.h"
#include "JsonObjectConverter.h"

FString UJsonParseLib::JsonParse(const FString& json)
{
	//�����⸦ �����
	TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(json);

	//�Ľ� ����� ���� ���� ����
	TSharedPtr<FJsonObject> result = MakeShareable(new FJsonObject());
	//�ؼ��� �Ѵ�.
	FString returnValue;
	if (FJsonSerializer::Deserialize(reader, result))
	{
		TArray<TSharedPtr<FJsonValue>> parseDataList = result->GetArrayField(TEXT("items"));
		for (TSharedPtr<FJsonValue> data : parseDataList)
		{
			//å�� ���� �̸�
			FString bookName = data->AsObject()->GetStringField("bk_nm");
			FString authorName = data->AsObject()->GetStringField("aut_nm");
			returnValue.Append(FString::Printf(TEXT("bookName %s / authorName %s\n"), *bookName, *authorName));
		}
	}
	//��ȯ�Ѵ�
	return returnValue;
}
FString UJsonParseLib::JsonParsePassword(const FString& json)
{
	//�����⸦ �����
	TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(json);

	//�Ľ� ����� ���� ���� ����
	TSharedPtr<FJsonObject> result = MakeShareable(new FJsonObject());
	//�ؼ��� �Ѵ�.
	FString returnValue;
	returnValue.Append(FString::Printf(TEXT("result %s\n"), *json));
	if (FJsonSerializer::Deserialize(reader, result))
	{

		TArray<TSharedPtr<FJsonValue>> parseDataList = result->GetArrayField(TEXT("items"));
		for (TSharedPtr<FJsonValue> data : parseDataList)
		{
			//å�� ���� �̸�
			FString bookName = data->AsObject()->GetStringField("bk_nm");
			FString authorName = data->AsObject()->GetStringField("aut_nm");
			returnValue.Append(FString::Printf(TEXT("bookName %s / authorName %s\n"), *bookName, *authorName));
		}
	}
	//��ȯ�Ѵ�
	return returnValue;
}

FString UJsonParseLib::MakeJson(const TMap<FString, FString> source)
{
	//source�� jsonObject�������� �����.
	TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
	for (TPair<FString, FString> pair : source)
	{
		jsonObject->SetStringField(pair.Key, pair.Value);
	}

	//writer�� ���� jsonobject�� ���ڵ��ؼ�
	FString json;
	TSharedRef<TJsonWriter<TCHAR>> writer = TJsonWriterFactory<TCHAR>::Create(&json);
	FJsonSerializer::Serialize(jsonObject.ToSharedRef(), writer);
	//��ȯ�Ѵ�
	return json;
}
