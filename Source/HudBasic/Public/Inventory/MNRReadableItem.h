// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/MNRItems.h"
#include "MNRReadableItem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class HUDBASIC_API UMNRReadableItem : public UMNRItems
{
	GENERATED_BODY()
	
public:

    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Readable")
    FText Title;

    // Multiline=true, It allows us to enter multi-line text in the Editor.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Readable", meta = (MultiLine = true))
    FText Content;

protected:

    virtual void Use(AActor* Instigator) override;

    virtual void ServerUse_Implementation(AActor* Instigator) override;

    virtual void ClientUse_Implementation(AActor* Instigator) override;

    virtual void OnRep_ItemUsed();


};
