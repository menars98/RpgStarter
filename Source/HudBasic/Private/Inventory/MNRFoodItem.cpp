// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/MNRFoodItem.h"

#include "Character/MNRHeroCharacter.h"
#include "Attributes/Abilities/MNRAbilitySystemComponent.h"
#include "Inventory/MNRInventoryComponent.h"

void UMNRFoodItem::Use(AMNRHeroCharacter* Character)
{

	if(Character)
	{
		//Gameplay effect for food items. 
		UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
		if(ASC && Character->IsAlive())
		{
			FGameplayEffectContextHandle GECH = ASC->MakeEffectContext();
			ASC->ApplyGameplayEffectToSelf(DefaultAttributes.GetDefaultObject(),0, GECH);

			if(OwningInventory)
			{
				OwningInventory->RemoveItem(this);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s is null"), *GetNameSafe(ASC));

		}
	}
}

