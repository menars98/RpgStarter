// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/MNRInventoryComponent.h"
#include "Inventory/MNRItems.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UMNRInventoryComponent::UMNRInventoryComponent()
{
	Capacity = 20;
	// Component must be replicated to replicate sub-objects
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UMNRInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

}

// Client or Server, everyone calls this function.
void UMNRInventoryComponent::TryAddItem(TSubclassOf<UMNRItems> ItemClass, int32 StackCount)
{
	// If this is the Server running this code, execute the direct insertion logic.
	if (GetOwner()->HasAuthority())
	{
		AddItem_Internal(ItemClass, StackCount);
	}
	// If it is the Client, send a request to the Server.
	else
	{
		Server_TryAddItem(ItemClass, StackCount);
	}
}

void UMNRInventoryComponent::TryRemoveItem(TSubclassOf<UMNRItems> ItemClass, int32 StackCount)
{
	if (GetOwner()->HasAuthority())
	{
		RemoveItem_Internal(ItemClass, StackCount);
	}
	else
	{
		Server_TryRemoveItem(ItemClass, StackCount);
	}
}


void UMNRInventoryComponent::Server_TryAddItem_Implementation(TSubclassOf<UMNRItems> ItemClass, int32 StackCount)
{
	AddItem_Internal(ItemClass, StackCount);
}

void UMNRInventoryComponent::Server_TryRemoveItem_Implementation(TSubclassOf<UMNRItems> ItemClass, int32 StackCount)
{
	RemoveItem_Internal(ItemClass, StackCount);
}

bool UMNRInventoryComponent::AddItem_Internal(TSubclassOf<UMNRItems> ItemClass, int32 StackCount)
{
	if (!ItemClass || StackCount <= 0)
	{
		return false;
	}

	AActor* OwningActor = GetOwner();
	if (!OwningActor)
	{
		return false;
	}

	// 2. Capacity Check: Is there space in the inventory?
	// Note: This check is only meaningful if a new slot is being added.
	// If we are adding to an existing item, we may not exceed the capacity.
	// Therefore, we can move this check after the stacking check.

	// 3. Stacking Check: Is this item already in the inventory?
	for (UMNRItems* ExistingItem : Items)
	{
		if (ExistingItem && ExistingItem->GetClass() == ItemClass)
		{
			ExistingItem->StackCount += StackCount;

			OnRep_Items();

			return true;
		}
	}

	// 4. Adding a New Slot: This item is not in the inventory, so we will add a new slot.
	// Now let's check the capacity again.
	if (Items.Num() >= Capacity)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory is full. Cannot add new item class: %s"), *ItemClass->GetName());
		return false;
	}

	// 5. Creating a New UObject
	// We set OwningActor as the Outer so its lifetime is tied to the character.
	UMNRItems* NewItem = NewObject<UMNRItems>(OwningActor, ItemClass);
	if (NewItem)
	{
		NewItem->StackCount = StackCount;
		NewItem->OwningInventory = this;
		NewItem->World = GetWorld();

		Items.Add(NewItem);

		OnRep_Items();

		return true;
	}

	return false;
}


bool UMNRInventoryComponent::RemoveItem_Internal(TSubclassOf<UMNRItems> ItemClass, int32 StackCount)
{
	// 1. Girdi Kontrolü
	if (!ItemClass || StackCount <= 0)
	{
		return false;
	}

	// 2. Kaldýrýlacak Item'ý Bul
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		UMNRItems* Item = Items[i];
		if (Item && Item->GetClass() == ItemClass)
		{
			// 3. Yýðýndan Çýkarma veya Tamamen Kaldýrma
			if (Item->StackCount > StackCount)
			{
				// Yýðýndan sadece bir kýsmýný çýkar.
				Item->StackCount -= StackCount;
			}
			else
			{
				// Ýstenen miktar, yýðýndaki miktara eþit veya daha fazla.
				// Tüm yýðýný kaldýr.
				Items.RemoveAt(i);

				// UObject'in artýk bir referansý kalmadýðý için Garbage Collector
				// tarafýndan temizlenmesini bekle. Ýstersen MarkAsGarbage() ile
				// bu süreci hýzlandýrabilirsin.
				Item->MarkAsGarbage();
			}

			// Sunucu tarafýnda UI'ýn anýnda güncellenmesi için OnRep'i manuel çaðýr.
			OnRep_Items();

			return true; // Ýþlem baþarýyla tamamlandý.
		}
	}

	// 4. Item Bulunamadý
	UE_LOG(LogTemp, Warning, TEXT("Could not find item class %s to remove."), *ItemClass->GetName());
	return false;
}

void UMNRInventoryComponent::OnRep_Items()
{
   OnInventoryUpdated.Broadcast(GetOwner());
}

// Specify that the Items array and the UObjects within it will be replicated.
void UMNRInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMNRInventoryComponent, Items);
}

