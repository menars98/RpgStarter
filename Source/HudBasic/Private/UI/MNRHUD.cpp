// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MNRHUD.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"


void AMNRHUD::ShowMainMenu()
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (MainMenu && MainMenu->IsInViewport())
	{
		MainMenu->RemoveFromParent();
		MainMenu = nullptr;

		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameAndUI());
		return;

	}

	// Make widget owned by our PlayerController
	MainMenu = CreateWidget<UUserWidget>(PC, MainMenuClass);

	if (MainMenu)
	{
		MainMenu->AddToViewport(100);

		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeGameOnly());
	}
}

void AMNRHUD::HideMainMenu()
{
	if (MainMenu)
	{
		MainMenu->RemoveFromParent();
		MainMenu = nullptr;
	}
}

bool AMNRHUD::IsMenuOpen() 
{
	if(MainMenu)
	{
		return true;
	}
	else
	{
		return false;
	}
}
