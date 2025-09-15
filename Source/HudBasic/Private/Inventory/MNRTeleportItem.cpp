// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/MNRTeleportItem.h"
#include <Character/MNRHeroCharacter.h>
#include "Inventory/MNRInventoryComponent.h"
#include <Character/MNRBaseCharacter.h>
#include <Kismet/GameplayStatics.h>
#include <MNRGameInstance.h>
#include <SaveLoad/MNRSaveGame.h>


void UMNRTeleportItem::Use(AActor* Instigator)
{
    Super::Use(Instigator);

    if (Instigator && Instigator->HasAuthority())
    {
        AMNRHeroCharacter* Hero = Cast<AMNRHeroCharacter>(Instigator);
        UMNRGameInstance* GameInstance = Cast<UMNRGameInstance>(UGameplayStatics::GetGameInstance(Instigator));

        if (Hero && GameInstance)
        {
            UMNRSaveGame* SaveData = Hero->GenerateSaveData();

            GameInstance->PlayerSaveData = SaveData;

            if (!NextLevelName.IsNone())
            {
                UGameplayStatics::OpenLevel(Instigator->GetWorld(), NextLevelName);
            }
        }
    }
}
