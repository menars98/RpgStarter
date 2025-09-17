// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/MNRItems.h"
#include "MNREquipmentItem.generated.h"




UCLASS()
class HUDBASIC_API UMNREquipmentItem : public UMNRItems
{
	GENERATED_BODY()
	
public:
    
    // This GameplayEffect will be applied when this equipment is equipped.
    // This effect grants stat bonuses and tags.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
    TSubclassOf<UGameplayEffect> EquipmentEffect;
    // The slot represented by this equipment (Head, Chest, etc.).
    // This is used to check whether something is already equipped in this slot.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
    FGameplayTag EquipmentSlotTag;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
    TObjectPtr<USkeletalMesh> EquipmentMesh;

    virtual void Use(AActor* Instigator) override;

};
