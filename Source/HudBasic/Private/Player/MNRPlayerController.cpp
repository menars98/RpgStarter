// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MNRPlayerController.h"

#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MNRPlayerState.h"
#include "UI/MNRBarWidget.h"
#include "UI/MNRHUD.h"

AMNRPlayerController::AMNRPlayerController()
{
}

void AMNRPlayerController::CreateHUD()
{
	// Only create once
	if (UIHUDWidget)
	{
		return;
	}

	if (!UIHUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing UIHUDWidgetClass. Please fill in on the Blueprint of the PlayerController."), *FString(__FUNCTION__));
		return;
	}

	// Only create a HUD for local player
	if (!IsLocalPlayerController())
	{
		return;
	}

	// Need a valid PlayerState to get attributes from
	AMNRPlayerState* PS = GetPlayerState<AMNRPlayerState>();
	if (!PS)
	{
		return;
	}

	UIHUDWidget = CreateWidget<UMNRBarWidget>(this, UIHUDWidgetClass);
	UIHUDWidget->AddToViewport();

	// Set attributes
	UIHUDWidget->SetCurrentHealth(PS->GetHealth());
	UIHUDWidget->SetMaxHealth(PS->GetMaxHealth());
	UIHUDWidget->SetHealthPercentage(PS->GetHealth() / FMath::Max<float>(PS->GetMaxHealth(), 1.f));
	UIHUDWidget->SetCurrentMana(PS->GetMana());
	UIHUDWidget->SetMaxMana(PS->GetMaxMana());
	UIHUDWidget->SetManaPercentage(PS->GetMana() / FMath::Max<float>(PS->GetMaxMana(), 1.f));
	UIHUDWidget->SetCurrentStamina(PS->GetStamina());
	UIHUDWidget->SetMaxStamina(PS->GetMaxStamina());
	UIHUDWidget->SetStaminaPercentage(PS->GetStamina() / FMath::Max<float>(PS->GetMaxStamina(), 1.f));
	UIHUDWidget->SetHeroLevel(PS->GetCharacterLevel());
}

UMNRBarWidget* AMNRPlayerController::GetHUDWidget()
{
	return UIHUDWidget;
}


void AMNRPlayerController::SetRespawnCountdown_Implementation(float RespawnTimeRemaining)
{
	if (UIHUDWidget)
	{
		UIHUDWidget->SetRespawnCountdown(RespawnTimeRemaining);
	}
}

bool AMNRPlayerController::SetRespawnCountdown_Validate(float RespawnTimeRemaining)
{
	return true;
}

void AMNRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("PauseMenu", IE_Pressed, this, &AMNRPlayerController::ToggleMenu);

}

void AMNRPlayerController::ToggleMenu()
{
	AMNRPlayerController* PC = Cast<AMNRPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	AMNRHUD* HUD = PC->GetHUD<AMNRHUD>();

	if(HUD)
	{
		HUD->ShowMainMenu();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Hud Is Empty"));
	}
		

	
}

void AMNRPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AMNRPlayerState* PS = GetPlayerState<AMNRPlayerState>();
	if (PS)
	{
		// Init ASC with PS (Owner) and our new Pawn (AvatarActor)
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}

void AMNRPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// For edge cases where the PlayerState is repped before the Hero is possessed.
	CreateHUD();
}
