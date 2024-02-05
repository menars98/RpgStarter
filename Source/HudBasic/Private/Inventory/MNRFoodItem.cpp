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
	if (Instigator->HasAuthority())
	{
		ServerUse_Implementation(Instigator);
	}
	else
	{
		ClientUse_Implementation(Instigator);
	}
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
					OwningInventory->ServerRemoveItem(this, Instigator);
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

void UMNRFoodItem::ClientUse_Implementation(AActor* Instigator)
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
					OwningInventory->ClientRemoveItem(this, Instigator);
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

void UMNRFoodItem::OnRep_ItemUsed()
{
	bool bDebugDraw = CVarDebugDrawASC.GetValueOnGameThread();
	AMNRHeroCharacter* Character = Cast<AMNRHeroCharacter>(GetOwningActor());
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
				if (Character->HasAuthority())
				{
					OwningInventory->ServerRemoveItem(this, GetOwningActor());
				}
				else
				{
					OwningInventory->ClientRemoveItem(this, GetOwningActor());
				}
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

