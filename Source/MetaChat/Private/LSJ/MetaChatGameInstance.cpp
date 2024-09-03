// Fill out your copyright notice in the Description page of Project Settings.


#include "LSJ/MetaChatGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
UMetaChatGameInstance::UMetaChatGameInstance()
	/*: CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete))
	, FindSessionCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionComplete))
	, JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplate))*/
{

}
void UMetaChatGameInstance::CreateGameSession()
{
	OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Subsystem Name : % s"), *OnlineSub->GetSubsystemName().ToString()));
	//	OnlineSessionInterface = OnlineSub->GetSessionInterface();
		auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
		if (ExistingSession != nullptr)
		{
			OnlineSessionInterface->DestroySession(NAME_GameSession);

			// Log
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Black, FString::Printf(TEXT("Destroy session : %s"), "1"));
			}
		}
		if (OnlineSessionInterface.IsValid())
		{
			TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
			SessionSettings->bIsLANMatch= false;
			// 게임 플레이어 총원
			SessionSettings->NumPublicConnections = 100;
			// 게임 세션 실행중이면 다른유저도 가입 가능한지
			SessionSettings->bAllowJoinInProgress = true;
			// 스팀이 게임 세션을 설정할 때 존재한다는 것?
			// 전 세계 모든 플레이어와의 연결이 아닌 지역이 있고 지역에서 발생하는 세션 검색하도록
			SessionSettings->bAllowJoinViaPresence = true;
			// 스팀이 세션을 광고해서 다른 유저들이 참여할 수 있다.
			SessionSettings->bShouldAdvertise = true;
			// 현재 지역에서 진행중인 세션을 찾는다
			SessionSettings->bUsesPresence = true;
			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);
			// 세션 생성 콜백을 위한 Delegate 바인딩
			OnlineSessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMetaChatGameInstance::OnCreateSessionComplete);
	
		}
	}
}

void UMetaChatGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("Session %s created successfully!"), *SessionName.ToString());
		GetWorld()->ServerTravel(FString("/Game/XR_LSJ/LSJMap?listen"));
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Session %s created successfully!"), *SessionName.ToString()));

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create session %s"), *SessionName.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Failed to create session %s"), *SessionName.ToString()));

	}
}
void UMetaChatGameInstance::JoinGameSession()
{
		if (OnlineSessionInterface.IsValid())
		{
			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			SessionSearch->MaxSearchResults = 10;	// 검색 결과로 나오는 세션 수 최대값
			SessionSearch->bIsLanQuery = false;			// LAN 사용 여부
			//SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals); // 찾을 세션 쿼리를 현재로 설정한다
		
			// 세션 검색 콜백을 위한 Delegate 바인딩
			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
				OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
	
		}
}
void UMetaChatGameInstance::DestroyGameSession()
{

		if (OnlineSessionInterface.IsValid())
		{
			OnlineSessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMetaChatGameInstance::OnDestroySessionComplete);
			OnlineSessionInterface->DestroySession(FName("My Session"));
		}
}
void UMetaChatGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
		if (OnlineSessionInterface.IsValid() && SessionSearch->SearchResults.Num() > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("Found %d sessions"), SessionSearch->SearchResults.Num());
			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSearch->SearchResults[0]);
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Found %d sessions"), 1));

		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No sessions found!"));
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("No sessions found!")));

		}

}
void UMetaChatGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Session %s destroyed successfully."), *SessionName.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Session %s destroyed successfully."), *SessionName.ToString()));

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to destroy session %s"), *SessionName.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Failed to destroy session %s"), *SessionName.ToString()));

	}
}
void UMetaChatGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully joined session %s"), *SessionName.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Successfully joined session %s"), *SessionName.ToString()));

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to join session %s"), *SessionName.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Failed to join session %s"), *SessionName.ToString()));
	}
}

void UMetaChatGameInstance::Init()
{
	Super::Init();
	// OnlineSubsystem에 Access
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		// 온라인 세션 받아오기
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
		OnlineSessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMetaChatGameInstance::OnFindSessionsComplete);

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Purple, FString::Printf(TEXT("Subsystem Name : % s"), *OnlineSubsystem->GetSubsystemName().ToString()));
	}
}
/*
void UMetaChatGameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	if (!OnlineSessionInterface.IsValid()
		|| !bWasSuccessful)
		return;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString(TEXT("======== Search Result ========")));
	}

	if (OnlineSessionInterface.IsValid() && SessionSearch->SearchResults.Num() > 0)
	{
		OnlineSessionInterface->JoinSession(0, NAME_GameSession, SessionSearch->SearchResults[0]);
	}

	for (auto Result : SessionSearch->SearchResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;

		// 매치 타입 확인하기
		FString MatchType;
		Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);

		// 찾은 세션의 정보 출력하기
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("Session ID : %s / Owner : %s"), *Id, *User));
		}

		// 세션의 매치 타입이 "FreeForAll"일 경우 세션 참가
		if (MatchType == FString("FreeForAll"))
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("Joining Match Type : %s"), *MatchType));
			}

			// Join Session Complete Delegate 등록 
			OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);


			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);
		}
	}
}

void UMetaChatGameInstance::CreateGameSession()
{
	// Called when pressing the 1key
	if (!OnlineSessionInterface.IsValid())
	{
		// log
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Game Session is invailed")));
		}
		return;
	}

	// 이미 세션이 존재한다면 기존 세션을 삭제한다
	auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);

		// Log
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Black, FString::Printf(TEXT("Destroy session : %s"), "NAME_GameSession"));
		}
	}

	// 세션 생성 완료 후 호출될 delegate 리스트에 CreateSessionCompleteDelegate 추가
	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// 세션 세팅하기
	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());

	SessionSettings->bIsLANMatch = true;			// LAN 연결
	SessionSettings->NumPublicConnections = 4;		// 최대 접속 가능 수
	SessionSettings->bAllowJoinInProgress = true;//Session 진행중에 접속 허용
	SessionSettings->bAllowJoinViaPresence = true;//세션 참가 지역을 현재 지역으로 제한 (스팀의 presence 사용)
	SessionSettings->bShouldAdvertise = true;		// 현재 세션을 광고할지 (스팀의 다른 플레이어에게 세션 홍보 여부)
	SessionSettings->bUsesPresence = true;			// 현재 지역에 세션 표시
	SessionSettings->bUseLobbiesIfAvailable = true; // 플랫폼이 지원하는 경우 로비 API 사용
	//SessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false; // NULL일 경우 LAN 연결
	SessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing); // 세션의 MatchType을 모두에게 열림, 온라인서비스와 핑을 통해 세션 홍보 옵션으로 설정

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	//OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);
	OnlineSessionInterface->CreateSession(0, NAME_GameSession, *SessionSettings);
}
void UMetaChatGameInstance::JoinGameSession()
{
	// 세션 인터페이스 유효성 검사
	if (!OnlineSessionInterface.IsValid())
	{
		// log
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Game Session Interface is invailed")));
		}
		return;
	}

	// Find Session Complete Delegate 등록
	OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegate);

	// Find Game Session
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 10000;	// 검색 결과로 나오는 세션 수 최대값
	SessionSearch->bIsLanQuery = true;			// LAN 사용 여부
	SessionSearch->PingBucketSize = 50;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals); // 찾을 세션 쿼리를 현재로 설정한다

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	//OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
	OnlineSessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}
void UMetaChatGameInstance::OnJoinSessionComplate(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!OnlineSessionInterface.IsValid())
		return;

	// 세션에 조인했다면 IP Address얻어와서 해당 서버에 접속
	FString Address;
	if (OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("Connect String : %s"), *Address));
		}

		APlayerController* PlayerController = GetFirstLocalPlayerController();
		if (PlayerController)
		{
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
}
void UMetaChatGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccssful)
{
	// 세션 생성 성공!
	if (bWasSuccssful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Created session : %s"), *SessionName.ToString()));
		}
	}

	// 세선 생성 실패
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Failed to create session!")));
		}
	}
}*/



/*
UMetaChatGameInstance::UMetaChatGameInstance()
	: CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete))
{
	// SessionInterface
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();

		if (GEngine)
		{
			// OnlineSubsystem 이름 출력하기
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Found subsystem %s"), *OnlineSubsystem->GetSubsystemName().ToString()));
		}
	}
}
void UMetaChatGameInstance::DestroySession()
{
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnDestroySessionComplete.Broadcast(false);
		return;
	}

	// Add DestroySessionCompleteDelegate to the session interface delegate list.
	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		// Clear the delegate from the delegate list
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		// Broadcast the result 
		MultiplayerOnDestroySessionComplete.Broadcast(false);
	}
}
void UMetaChatGameInstance::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	if (!SessionInterface.IsValid())
		return;

	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = NumPublicConnections;
		LastMatchType = MatchType;

		DestroySession();
	}

	// Add CreateSessionCompleteDelegate to the session interface delegate list.
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// Setting options for a new session
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false; // NULL일 경우 LAN 연결
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LastSessionSettings->BuildUniqueId = 1;

	// Create Session
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		// Remove the delegate from the delegate list using the handle.
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

		// Broadcast that the session creation has failed.
		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}
}
void UMetaChatGameInstance::FindSession(int32 MaxSearchResults)
{
	if (!SessionInterface.IsValid())
		return;

	// Add FindSessionCompleteDelegate to the session interface delegate list.
	FindSessionCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegate);

	// Setting session search options
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch);
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// Find sessions
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		// Clear the delegate from the delegate list
		SessionInterface->ClearOnCancelFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegateHandle);
		// Broadcast the result 
		MultiplayerOnFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);

	}

}
void UMetaChatGameInstance::Init()
{
	IOnlineSubsystem* OnlineSubsystemInterface = IOnlineSubsystem::Get();
	if (nullptr != OnlineSubsystemInterface)
	{
		SessionInterface = OnlineSubsystemInterface->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMetaChatGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMetaChatGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMetaChatGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMetaChatGameInstance::OnJoinSessionComplete);

		}
	}
}
*/