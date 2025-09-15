// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MNRGameInstance.generated.h"


class UMNRSaveGame;

UCLASS()
class HUDBASIC_API UMNRGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    // Seviyeler arasýnda taþýnacak olan tek veri nesnesi.
    UPROPERTY(BlueprintReadWrite, Category = "Save Data")
    TObjectPtr<UMNRSaveGame> PlayerSaveData;
};
