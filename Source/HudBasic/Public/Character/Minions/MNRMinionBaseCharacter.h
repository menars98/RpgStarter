// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MNRBaseCharacter.h"
#include "GameplayEffectTypes.h"
#include "MNRMinionBaseCharacter.generated.h"

/**
 * 
 */
UCLASS()
class HUDBASIC_API AMNRMinionBaseCharacter : public AMNRBaseCharacter
{
	GENERATED_BODY()
	
public:

	AMNRMinionBaseCharacter(const class FObjectInitializer& ObjectInitializer);

protected:

	// Actual hard pointer to AbilitySystemComponent
	UPROPERTY()
	class UMNRAbilitySystemComponent* HardRefAbilitySystemComponent;

	// Actual hard pointer to AttributeSetBase
	UPROPERTY()
	class UMNRAttributeSetBase* HardRefAttributeSetBase;

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	TSubclassOf<class UMNRFloatingStatusBarWidget> UIFloatingStatusBarClass;

	UPROPERTY()
	class UMNRFloatingStatusBarWidget* UIFloatingStatusBar;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "|UI")
	class UWidgetComponent* UIFloatingStatusBarComponent;

	FDelegateHandle HealthChangedDelegateHandle;

	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);

	// Tag change callbacks
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI|Abilities")
	void OnHealthChanged(float NewHealth, float MaxHealth);

};
