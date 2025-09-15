// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include <MNRGameInstance.h>
#include <HudBasic/HudBasic.h>
#include "MNRSaveGame.generated.h"

class UMNRItems;

USTRUCT(BlueprintType)
struct FInventorySlotData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class UMNRItems> ItemClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 StackCount;
};

UCLASS()
class HUDBASIC_API UMNRSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

    UPROPERTY()
    TArray<FInventorySlotData> SavedInventoryItems;

    UPROPERTY()
    TMap<EEquipmentSlot, TSubclassOf<UMNRItems>> SavedEquippedItems; 

    UPROPERTY()
    float SavedHealth;

    UPROPERTY()
    FString PlayerName;

    //Other save datas such as; mana, gold etc.
};
