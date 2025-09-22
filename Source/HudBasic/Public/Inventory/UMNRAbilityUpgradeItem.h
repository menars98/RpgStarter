// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/MNRItems.h"
#include "UMNRAbilityUpgradeItem.generated.h"

/**
 * 
 */
UCLASS()
class HUDBASIC_API UUMNRAbilityUpgradeItem : public UMNRItems
{
	GENERATED_BODY()
	
public:

    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade")
    TSubclassOf<UGameplayAbility> AbilityToUpgrade;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Upgrade", meta = (ClampMin = 1))
    int32 NewLevel = 1;

    virtual void Use(AActor* Instigator) override;
};
