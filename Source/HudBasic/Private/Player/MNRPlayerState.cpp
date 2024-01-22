// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MNRPlayerState.h"

#include "AbilitySystemComponent.h"
#include "Attributes/MNRAttributeSetBase.h"
#include "Attributes/Abilities/MNRAbilitySystemComponent.h"
#include "Character/MNRBaseCharacter.h"
#include "Character/MNRHeroCharacter.h"
#include "Player/MNRPlayerController.h"
#include "UI/MNRBarWidget.h"
#include "UI/MNRFloatingStatusBarWidget.h"

AMNRPlayerState::AMNRPlayerState()
{
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UMNRAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSetBase = CreateDefaultSubobject<UMNRAttributeSetBase>(TEXT("AttributeSetBase"));

	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	// 100 is probably way too high for a shipping game, you can adjust to fit your needs.
	NetUpdateFrequency = 100.0f;

	// Cache tags
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

UAbilitySystemComponent* AMNRPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UMNRAttributeSetBase* AMNRPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

bool AMNRPlayerState::IsAlive() const
{
	return GetHealth() > 0.0f;
}

void AMNRPlayerState::ShowAbilityConfirmCancelText(bool ShowText)
{
	
	AMNRPlayerController* PC = Cast<AMNRPlayerController>(GetOwner());
	if (PC)
	{
		UMNRBarWidget* HudWidget = PC->GetHUDWidget();
		if (HudWidget)
		{
			HudWidget->ShowAbilityConfirmCancelText(ShowText);
			UE_LOG(LogTemp, Warning, TEXT("Hud to widget working"))
		}
		UE_LOG(LogTemp, Warning, TEXT("Hud to widget not working"))
	}
}

float AMNRPlayerState::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

float AMNRPlayerState::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

float AMNRPlayerState::GetMana() const
{
	return AttributeSetBase->GetMana();
}

float AMNRPlayerState::GetMaxMana() const
{
	return AttributeSetBase->GetMaxMana();
}

float AMNRPlayerState::GetStamina() const
{
	return AttributeSetBase->GetStamina();
}

float AMNRPlayerState::GetMaxStamina() const
{
	return AttributeSetBase->GetMaxStamina();
}

int32 AMNRPlayerState::GetCharacterLevel() const
{
	return AttributeSetBase->GetCharacterLevel();
}

void AMNRPlayerState::BeginPlay()
{
	Super::BeginPlay();

	HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AMNRPlayerState::HealthChanged);
	MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxHealthAttribute()).AddUObject(this, &AMNRPlayerState::MaxHealthChanged);
	ManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetManaAttribute()).AddUObject(this, &AMNRPlayerState::ManaChanged);
	MaxManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxManaAttribute()).AddUObject(this, &AMNRPlayerState::MaxManaChanged);
	StaminaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetStaminaAttribute()).AddUObject(this, &AMNRPlayerState::StaminaChanged);
	MaxStaminaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxStaminaAttribute()).AddUObject(this, &AMNRPlayerState::MaxStaminaChanged);
	CharacterLevelChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetCharacterLevelAttribute()).AddUObject(this, &AMNRPlayerState::CharacterLevelChanged);

	// Tag change callbacks
	AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AMNRPlayerState::StunTagChanged);
}

void AMNRPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;

	// Update floating status bar
	AMNRHeroCharacter* Hero = Cast<AMNRHeroCharacter>(GetPawn()); // We need to seperate Basecharacter & Herocharacter If we want diversion
	if (Hero)
	{
		UMNRFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetHealthPercentage(Health / GetMaxHealth());
		}
	}

	// Update the HUD
	// Can be Handled in the UI itself using the AsyncTaskAttributeChanged node as an example how to do it in Blueprint
	AMNRPlayerController* PC = Cast<AMNRPlayerController>(GetOwner());
	if (PC)
	{
		UMNRBarWidget* HudWidget = PC->GetHUDWidget();
		if (HudWidget)
		{
			HudWidget->SetCurrentHealth(Health);
		}
	}

	// If the player died, handle death
	if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	{
		if (Hero)
		{
			Hero->Die();
		}
	}
}

void AMNRPlayerState::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	float MaxHealth = Data.NewValue;

	// Update floating status bar
	AMNRHeroCharacter* Hero = Cast<AMNRHeroCharacter>(GetPawn());
	if (Hero)
	{
		UMNRFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetHealthPercentage(GetHealth() / MaxHealth);
		}
	}

	// Update the HUD
	AMNRPlayerController* PC = Cast<AMNRPlayerController>(GetOwner());
	if (PC)
	{
		UMNRBarWidget* HudWidget = PC->GetHUDWidget();
		if (HudWidget)
		{
			HudWidget->SetMaxHealth(MaxHealth);
		}
	}
}

void AMNRPlayerState::ManaChanged(const FOnAttributeChangeData& Data)
{
	float Mana = Data.NewValue;

	// Update floating status bar
	AMNRHeroCharacter* Hero = Cast<AMNRHeroCharacter>(GetPawn());
	if (Hero)
	{
		UMNRFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetManaPercentage(Mana / GetMaxMana());
		}
	}

	// Update the HUD
	// Can be Handled in the UI itself using the AsyncTaskAttributeChanged node as an example how to do it in Blueprint
	AMNRPlayerController* PC = Cast<AMNRPlayerController>(GetOwner());
	if (PC)
	{
		UMNRBarWidget* HudWidget = PC->GetHUDWidget();
		if (HudWidget)
		{
			HudWidget->SetCurrentMana(Mana);
		}
	}
}

void AMNRPlayerState::MaxManaChanged(const FOnAttributeChangeData& Data)
{
	float MaxMana = Data.NewValue;

	// Update floating status bar
	AMNRHeroCharacter* Hero = Cast<AMNRHeroCharacter>(GetPawn());
	if (Hero)
	{
		UMNRFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetManaPercentage(GetMana() / MaxMana);
		}
	}

	// Update the HUD
	AMNRPlayerController* PC = Cast<AMNRPlayerController>(GetOwner());
	if (PC)
	{
		UMNRBarWidget* HudWidget = PC->GetHUDWidget();
		if (HudWidget)
		{
			HudWidget->SetMaxMana(MaxMana);
		}
	}
}

void AMNRPlayerState::StaminaChanged(const FOnAttributeChangeData& Data)
{
	float Stamina = Data.NewValue;

	// Update the HUD
	// Can be Handled in the UI itself using the AsyncTaskAttributeChanged node as an example how to do it in Blueprint
	AMNRPlayerController* PC = Cast<AMNRPlayerController>(GetOwner());
	if (PC)
	{
		UMNRBarWidget* HudWidget = PC->GetHUDWidget();
		if (HudWidget)
		{
			HudWidget->SetCurrentStamina(Stamina);
		}
	}
}

void AMNRPlayerState::MaxStaminaChanged(const FOnAttributeChangeData& Data)
{
	float MaxStamina = Data.NewValue;

	// Update the HUD
	AMNRPlayerController* PC = Cast<AMNRPlayerController>(GetOwner());
	if (PC)
	{
		UMNRBarWidget* HUD = PC->GetHUDWidget();
		if (HUD)
		{
			HUD->SetMaxStamina(MaxStamina);
		}
	}
}

void AMNRPlayerState::CharacterLevelChanged(const FOnAttributeChangeData& Data)
{
	float CharacterLevel = Data.NewValue;

	// Update the HUD
	AMNRPlayerController* PC = Cast<AMNRPlayerController>(GetOwner());
	if (PC)
	{
		UMNRBarWidget* HUD = PC->GetHUDWidget();
		if (HUD)
		{
			HUD->SetHeroLevel(CharacterLevel);
		}
	}
}

void AMNRPlayerState::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		FGameplayTagContainer AbilityTagsToCancel;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

		FGameplayTagContainer AbilityTagsToIgnore;
		AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCanceledByStun")));

		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);
	}
}
