// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MNRPlayerController.generated.h"

class AMNRHUD;
/**
 * 
 */
UCLASS()
class HUDBASIC_API AMNRPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AMNRPlayerController();

	void CreateHUD();

	class UMNRBarWidget* GetHUDWidget();

	// Simple way to RPC to the client the countdown until they respawn from the GameMode. Will be latency amount of out sync with the Server.
	UFUNCTION(Client, Reliable, WithValidation)
	void SetRespawnCountdown(float RespawnTimeRemaining);
	void SetRespawnCountdown_Implementation(float RespawnTimeRemaining);
	bool SetRespawnCountdown_Validate(float RespawnTimeRemaining);

protected:

	virtual void SetupInputComponent() override;

	void ToggleMenu();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASDocumentation|UI")
	TSubclassOf<class UMNRBarWidget> UIHUDWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "GASDocumentation|UI")
	class UMNRBarWidget* UIHUDWidget;

	UPROPERTY()
	AMNRHUD* PlayerHUD;

	// Server only
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnRep_PlayerState() override;
};
