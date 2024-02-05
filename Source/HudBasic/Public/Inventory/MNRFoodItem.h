// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/MNRItems.h"
#include "MNRFoodItem.generated.h"

/**
 * 
 */
UCLASS()
class HUDBASIC_API UMNRFoodItem : public UMNRItems
{
	GENERATED_BODY()

protected:

	/*Default Attributes for Items*/
	//@TODO add array for several attributes
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	virtual void Use(AActor* Instigator) override;

	virtual void ServerUse_Implementation(AActor* Instigator) override;

	virtual void ClientUse_Implementation(AActor* Instigator) override;

	virtual void OnRep_ItemUsed();
};
