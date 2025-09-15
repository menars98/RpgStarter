// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/MNRItems.h"
#include "MNRTeleportItem.generated.h"

/**
 * 
 */
UCLASS()
class HUDBASIC_API UMNRTeleportItem : public UMNRItems
{
	GENERATED_BODY()
	
public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Teleport")
    FName NextLevelName;

    virtual void Use(AActor* Instigator) override;

};
