// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/MNREquipmentItem.h"
#include "Character/MNRBaseCharacter.h"
#include "Character/MNRHeroCharacter.h"
#include "Attributes/Abilities/MNRAbilitySystemComponent.h"
#include "Attributes/Abilities/MNRBaseGameplayAbility.h"
#include "Config/MNRGameplayTags.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"


void UMNREquipmentItem::Use(AActor* Instigator)
{
    Super::Use(Instigator);

	AMNRHeroCharacter* Character = Cast<AMNRHeroCharacter>(Instigator);

    if (!Character || !EquipmentEffect) return;

    UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
    if (!ASC) return;

    // Get the default object (CDO) of the GameplayEffect class.
    const UGameplayEffect* EffectCDO = GetDefault<UGameplayEffect>(EquipmentEffect);
    if (!EffectCDO) return;

    const FGameplayTagContainer EffectGrantedTags = EffectCDO->GetGrantedTags();

    // Assumption: Each equipment effect provides a SINGLE tag that defines that equipment.
    // We use this tag as the unique identifier for this equipment.
    if (EffectGrantedTags.Num() > 0)
    {
        // We are retrieving the first tag from the tag container.
        // This will be the “Equipment.Head.HelmetA” tag inside GE_Equip_HelmetA.
        const FGameplayTag& EquipmentInstanceTag = *EffectGrantedTags.CreateConstIterator();

        // Now check: Does this unique ID tag already exist in the character?
        if (ASC->HasMatchingGameplayTag(EquipmentInstanceTag))
        {
            // YES, IT'S EQUIPPED: This means the item is already equipped.
            // Then perform the removal operation.
            Character->UnequipItemFromSlot(EquipmentSlotTag);
        }
        else
        {
			// No, the item is not currently equipped.
            Character->EquipItem(this);
        }
    }
    else
    {
        // Warning: This equipment effect does not assign any tags to the character.
        // This means we cannot track the equipped/unequipped status.
        UE_LOG(LogTemp, Warning, TEXT("EquipmentEffect %s in item %s has no Granted Tags! Equip/unequip logic may fail."),
            *GetNameSafe(EquipmentEffect), *GetName());

        // Even without a label, we can try to wear it, but this is not recommended.
        Character->EquipItem(this);
    }
}
