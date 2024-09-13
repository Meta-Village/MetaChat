// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MetaChatGameInstance.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSuccesssful);
//DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bwasSuccessful);
//DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionComplete, bool, bWasSuccessful);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionComplete, bool, bWasSuccessful);

/**
 * 
 */
UCLASS()
class METACHAT_API UMetaChatGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UMetaChatGameInstance();
	class IOnlineSubsystem* OnlineSub;
	IOnlineSessionPtr OnlineSessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	//FOnCreateSessionCompleteDelegate	CreateSessionCompleteDelegate;
	//FOnFindSessionsCompleteDelegate		FindSessionCompleteDelegate;
	//FOnJoinSessionCompleteDelegate		JoinSessionCompleteDelegate;
	//FOnJoinSessionCompleteDelegate		JoinSessionCompleteDelegate;
	//FOnStartSessionCompleteDelegate		StartSessionCompletetDelegate;
	//FOnDestroySessionCompleteDelegate	DestroySessionCompleteDelegate;
	void JoinGameSession();
	void DestroyGameSession();
	void CreateGameSession();
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	virtual void Init();

	//캐릭터 정보
	UPROPERTY(BlueprintReadWrite)
	FString UserID;
	UPROPERTY(BlueprintReadWrite)
	FString UserToken;
	UPROPERTY(BlueprintReadWrite)
	int32 WorldID;
	/*
	void OnJoinSessionComplate(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccssful);
	void JoinGameSession();
	void CreateGameSession();
	*/



	/**
 *

	bool bCreateSessionOnDestroy = false;
	int32 LastNumPublicConnections;
	FString LastMatchType;
	TSharedPtr<class FOnlineSessionSettings>	LastSessionSettings;
	TSharedPtr<class FOnlineSessionSearch>	LastSessionSearch;
protected:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
public:
	UMetaChatGameInstance();
	void DestroySession();
	void CreateSession(int32 NumPublicConnections, FString MatchType);
	void Init();

	void FindSession(int32 MaxSearchResults);
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	FOnCreateSessionCompleteDelegate	CreateSessionCompleteDelegate;
	FDelegateHandle						CreateSessionCompleteDelegateHandle;
	FOnFindSessionsCompleteDelegate		FindSessionCompleteDelegate;
	FDelegateHandle						FindSessionCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate		JoinSessionCompleteDelegate;
	FDelegateHandle						JoinSessionCompleteDelegateHandle;
	FOnStartSessionCompleteDelegate		StartSessionCompletetDelegate;
	FDelegateHandle						StartSessionCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate	DestroySessionCompleteDelegate;
	FDelegateHandle						DestroySessionCompleteDelegateHandle;

	FMultiplayerOnCreateSessionComplete MultiplayerOnCreateSessionComplete;
	FMultiplayerOnFindSessionComplete MultiplayerOnFindSessionComplete;
	FMultiplayerOnJoinSessionComplete MultiplayerOnJoinSessionComplete;
	FMultiplayerOnStartSessionComplete MultiplayerOnStartSessionComplete;
	FMultiplayerOnDestroySessionComplete MultiplayerOnDestroySessionComplete;
	 */
};
