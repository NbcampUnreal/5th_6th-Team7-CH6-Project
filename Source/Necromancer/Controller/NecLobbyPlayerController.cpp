// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/NecLobbyPlayerController.h"

#include "Interfaces/OnlineExternalUIInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"

#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

#include "Online/OnlineSessionNames.h" 
#include "OnlineSessionSettings.h"

ANecLobbyPlayerController::ANecLobbyPlayerController()
{

}

void ANecLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}

	if (IsValid(LobbyWidgetClass))
	{
		LobbyWidgetInstance = CreateWidget<UUserWidget>(this, LobbyWidgetClass);
		if (IsValid(LobbyWidgetInstance))
		{
			LobbyWidgetInstance->AddToViewport();

			FInputModeUIOnly Mode;
			Mode.SetWidgetToFocus(LobbyWidgetInstance->GetCachedWidget());
			SetInputMode(Mode);

			bShowMouseCursor = true;
		}
	}

	//Login();
}

void ANecLobbyPlayerController::GetOnlineSubsystem()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	if (OnlineSubsystem)
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();


		if (OnlineSessionInterface)
		{
			OnlineSessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &ANecLobbyPlayerController::OnInviteAccepted);
		}
	}
}

void ANecLobbyPlayerController::Login()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem) {
		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			FOnlineAccountCredentials Credentials;
			Credentials.Id = FString();
			Credentials.Token = FString();
			Credentials.Type = FString("accountportal");

			Identity->OnLoginCompleteDelegates->AddUObject(this, &ANecLobbyPlayerController::OnLoginComplete);
			Identity->Login(0, Credentials);
		}
	}
}

void ANecLobbyPlayerController::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	UE_LOG(LogTemp, Warning, TEXT("Logged In %d"), bWasSuccessful);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Logged In % d"), bWasSuccessful));
	}

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		if (IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			bIsLoggedIn = bWasSuccessful;
			Identity->ClearOnLoginCompleteDelegates(0, this);
		}
	}
}

void ANecLobbyPlayerController::OnClickCreateSession()
{
	if (!OnlineSessionInterface.IsValid())
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Game Session Interface is invailed")));

		return;
	}

	auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Black, FString::Printf(TEXT("Destroy session")));
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Create Session...")));
	}

	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);
	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());

	SessionSettings->bIsLANMatch = false;
	SessionSettings->NumPublicConnections = 4;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bUseLobbiesIfAvailable = true;

	//SessionSettings->Set(FName(TEXT("SessionName")), FString(TEXT("Necromancer"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing));

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);
}

void ANecLobbyPlayerController::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	//UE_LOG(LogTemp, Error, TEXT("Complete Create Seesion : % s"), *SessionName.ToString());

	if (!bWasSuccessful || !OnlineSessionInterface.IsValid()) return;

	FString Address;
	if (OnlineSessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan,
				FString::Printf(TEXT("Server Connect String: %s"), *Address));
		}
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Complete Create Seesion : % s"), *SessionName.ToString()));
	}

	FNamedOnlineSession* Session = OnlineSessionInterface->GetNamedSession(SessionName);
	if (Session)
	{
		GetWorld()->ServerTravel(TEXT("/Game/Necromancer/Maps/TestThirdPersonMap?listen"));
	}
}

void ANecLobbyPlayerController::OnClickFindSession()
{
	if (!OnlineSessionInterface.IsValid())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Game Session Interface is invailed")));
		}

		return;
	}

	if (FindSessionsCompleteDelegateHandle.IsValid() == false)
	{
		FindSessionsCompleteDelegateHandle = OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegate);
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->bIsLanQuery = false;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	if (GEngine)
	{
		IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("Find Sessions...")));
	}

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void ANecLobbyPlayerController::OnFindSessionComplete(bool bWasSuccessful)
{
	if (!OnlineSessionInterface.IsValid() || !bWasSuccessful)
	{
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("Found Session Count : %d"), SessionSearch->SearchResults.Num());
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("Found Session Count : %d"), SessionSearch->SearchResults.Num()));
	}

	for (auto Result : SessionSearch->SearchResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;

		FString SessionName;
		//Result.Session.SessionSettings.Get(FName("SessionName"), SessionName);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("%s Session: %s(%s)"), *User, *SessionName, *Id));
		}

		OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

		const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		if (IsValid(LocalPlayer))
		{
			OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);
			return;
		}
	}

}

void ANecLobbyPlayerController::OnJoinSessionComplate(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan,
			FString::Printf(TEXT("OnJoinSessionComplete: Name=%s, Result=%d"), *SessionName.ToString(), (int32)Result));
	}

	FString Address;
	if (OnlineSessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("Joined Session Name : %s(%s)"), *SessionName.ToString(), *Address));
		}

		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		if (PlayerController)
		{
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
}

void ANecLobbyPlayerController::OnClickInviteFriend()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineExternalUIPtr ExternalUI = Subsystem->GetExternalUIInterface();
		if (ExternalUI.IsValid())
		{
			ExternalUI->ShowInviteUI(0, NAME_GameSession);
		}
	}
}

void ANecLobbyPlayerController::OnInviteAccepted(bool bWasSuccessful, int32 LocalUserNum, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult)
{
	if (bWasSuccessful && InviteResult.IsValid())
	{
		if (GEngine) 
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("Invite Accepted! Joining..."));
		}

		FString OwningUser = InviteResult.Session.OwningUserName;
		FString SessionId = InviteResult.GetSessionIdStr();

		if (GEngine)
		{

			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, FString::Printf(TEXT("Invite From: %s, Session ID: %s"), *OwningUser, *SessionId));
		}

		OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
		const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
		if (LocalPlayer)
		{
			OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(),  FName(TEXT("Necromancer")), InviteResult);
		}
	}
}
