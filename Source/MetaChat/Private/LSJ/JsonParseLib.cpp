// Fill out your copyright notice in the Description page of Project Settings.


#include "LSJ/JsonParseLib.h"
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

void UJsonParseLib::JsonParsePassword(const FString& json,int32& WorldID,FString WorldName)
{
	// JSON 파싱을 위한 Reader 생성
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(json);

    // JSON 객체를 저장할 포인터 선언
    TSharedPtr<FJsonObject> JsonObject;

    // JSON 문자열을 파싱하여 JsonObject에 저장
    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        // "월드 ID" 필드를 int32 타입으로 추출
         WorldID = JsonObject->GetIntegerField(TEXT("worldId"));

        // "월드 이름" 필드를 FString 타입으로 추출
         WorldName = JsonObject->GetStringField(TEXT("worldName"));

        // 출력 로그
        UE_LOG(LogTemp, Log, TEXT("월드 ID: %d"), WorldID);
        UE_LOG(LogTemp, Log, TEXT("월드 이름: %s"), *WorldName);
    }
    else
    {
        // 파싱 실패 시 로그 출력
        UE_LOG(LogTemp, Error, TEXT("JSON 파싱 실패"));
    }

}
bool UJsonParseLib::JsonParseUserInfo(const FString& json, FString& UserToken)
{
	// JSON 파싱을 위한 Reader 생성
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(json);

    // JSON 객체를 저장할 포인터 선언
    TSharedPtr<FJsonObject> JsonObject;

    // JSON 문자열을 파싱하여 JsonObject에 저장
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        // "message" 값 가져오기
        FString Message = JsonObject->GetStringField(TEXT("message"));

        // "userInfo" 객체 가져오기
        TSharedPtr<FJsonObject> UserInfo = JsonObject->GetObjectField(TEXT("userInfo"));
        if (UserInfo.IsValid())
        {
            // "userId" 값 가져오기
            FString UserId = UserInfo->GetStringField(TEXT("userId"));

            // "userNo" 값 가져오기
            int32 UserNo = UserInfo->GetIntegerField(TEXT("userNo"));

            // "userPass" 값 가져오기
            UserToken = UserInfo->GetStringField(TEXT("userPass"));

            // 여기서 추출된 데이터를 사용하거나 처리 가능
            UE_LOG(LogTemp, Log, TEXT("Message: %s"), *Message);
            UE_LOG(LogTemp, Log, TEXT("UserId: %s"), *UserId);
            UE_LOG(LogTemp, Log, TEXT("UserNo: %d"), UserNo);
            UE_LOG(LogTemp, Log, TEXT("UserPass: %s"), *UserToken);
        }
		return true;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to parse JSON"));
    }
	return false;
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
