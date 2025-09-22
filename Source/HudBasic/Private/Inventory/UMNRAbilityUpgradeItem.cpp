// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UMNRAbilityUpgradeItem.h"
#include <Character/MNRHeroCharacter.h>
#include "Inventory/MNRInventoryComponent.h"


void UUMNRAbilityUpgradeItem::Use(AActor* Instigator)
{
    Super::Use(Instigator);

    AMNRHeroCharacter* Character = Cast<AMNRHeroCharacter>(Instigator);
    if (Character)
    {
        // Artýk sadece yükseltilecek yeteneði söylüyoruz.
        Character->UpgradeAbility(AbilityToUpgrade);

        if (OwningInventory)
        {
            OwningInventory->TryRemoveItemInstance(this);
        }
    }
}
