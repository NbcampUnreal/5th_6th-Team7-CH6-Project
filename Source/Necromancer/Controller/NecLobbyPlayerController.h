// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "Interfaces/OnlineSessionInterface.h"

#include "NecLobbyPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class NECROMANCER_API ANecLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANecLobbyPlayerController();
	virtual void BeginPlay() override;


private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ASUIPlayerController, Meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> LobbyWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = ASUIPlayerController, Meta = (AllowPrivateAccess))
	TObjectPtr<UUserWidget> LobbyWidgetInstance;

protected:
	void GetOnlineSubsystem();

	TSharedPtr<class IOnlineSession, ESPMode::ThreadSafe> OnlineSessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;


public:
	void Login();
	void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
protected:
	bool bIsLoggedIn;



	UPROPERTY(EditDefaultsOnly, Category = "BGM")
	TObjectPtr<USoundBase> TitleBGM;

# pragma region CreateSession
public:
	UFUNCTION(BlueprintCallable)
	void OnClickCreateSession();

private:
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

#pragma endregion


# pragma region FindSession
public:
	UFUNCTION(BlueprintCallable)
	void OnClickFindSession();

private:
	FOnFindSessionsCompleteDelegate FindSessionCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	void OnFindSessionComplete(bool bWasSuccessful);

#pragma endregion

# pragma region JoinSession

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	void OnJoinSessionComplate(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

#pragma endregion


# pragma region Invite Friend


public:
	UFUNCTION(BlueprintCallable)
	void OnClickInviteFriend();

	void OnInviteAccepted(bool bWasSuccessful, int32 LocalUserNum, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& InviteResult);

private:
	

#pragma endregion
	
};
