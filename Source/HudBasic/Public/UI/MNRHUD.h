// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MNRHUD.generated.h"


/**
 * 
 */
UCLASS()
class HUDBASIC_API AMNRHUD : public AHUD
{
	GENERATED_BODY()
public:
	// Make BlueprintCallable for testing
	UFUNCTION(BlueprintCallable)
	void ShowMainMenu();
	UFUNCTION(BlueprintCallable)
	void HideMainMenu();

	//Look for Main Menu
	UFUNCTION(BlueprintCallable)
	bool IsMenuOpen();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> MainMenuClass;

	// Keep a pointer to be able to hide it
	UPROPERTY()
	class UUserWidget* MainMenu;
};
