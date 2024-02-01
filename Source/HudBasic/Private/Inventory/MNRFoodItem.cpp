// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/MNRFoodItem.h"

#include "Attributes/MNRAttributeSetBase.h"
#include "Character/MNRHeroCharacter.h"
#include "Attributes/Abilities/MNRAbilitySystemComponent.h"
#include "Inventory/MNRInventoryComponent.h"
#include "DrawDebugHelpers.h"

static TAutoConsoleVariable<bool> CVarDebugDrawASC(TEXT("mnr.ASCDebug"), false, TEXT("Enable Debug for Ability System Component."), ECVF_Cheat);

void UMNRFoodItem::Use(AActor* Instigator)
{
	ServerUse_Implementation(Instigator);
}

void UMNRFoodItem::ServerUse_Implementation(AActor* Instigator)
{
	bool bDebugDraw = CVarDebugDrawASC.GetValueOnGameThread();
	AMNRHeroCharacter* Character = Cast<AMNRHeroCharacter>(Instigator);
	if (Character)
	{
		//Gameplay effect for food items.
		UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();

		if (ASC && Character->IsAlive())
		{
			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle NewHandle = ASC->MakeOutgoingSpec(DefaultAttributes, Character->AttributeSetBase->GetCharacterLevel(), EffectContext);
			if (NewHandle.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), Character->AbilitySystemComponent.Get());
			}

			if (OwningInventory)
			{
				OwningInventory->ServerRemoveItem(this);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("%s is null"), *GetNameSafe(ASC));

		}
		if (bDebugDraw)
		{
			GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, (TEXT("%s: is asc"), *GetNameSafe(ASC)));
			GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, (TEXT("%s: is Character"), *GetNameSafe(Character)));

		}
	}
}

