// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/MNRInventoryComponent.h"
#include "Inventory/MNRItems.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UMNRInventoryComponent::UMNRInventoryComponent()
{
	Capaticy = 20;
	// Component must be replicated to replicate sub-objects
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UMNRInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	for(auto& Item : DefaultItems)
	{
		AddItem(Item, this->GetOwner());
	}
	
}

bool UMNRInventoryComponent::AddItem(UMNRItems* Item, AActor* OwningActor)
{
	if(Items.Num() >= Capaticy || !Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is null or %c: is greater than %c"), *GetNameSafe(Item), Items.Num(), Capaticy);
		return false;
	}
	bool bIsNewItem = true;
	for(auto& ItemVar: Items)
	{
		if(ItemVar->ItemClass.GetDefaultObject() == Item->ItemClass.GetDefaultObject())
		{
			++ItemVar->StackCount;
			bIsNewItem = false;
			break;
		}
	}
	if(bIsNewItem)
	{
		Item->OwningInventory = this;
		Item->World = GetWorld();
		Items.Add(Item);
		//Update UI
		OnInventoryUpdated.Broadcast(OwningActor);
	}
	else
	{
		OnInventoryUpdated.Broadcast(OwningActor);
	}
	return true;
}

bool UMNRInventoryComponent::RemoveItem(UMNRItems* Item, AActor* OwningActor)
{
	if(Item->StackCount <1)
	{
		Item->OwningInventory = nullptr;
		Item->World = nullptr;
		Items.RemoveSingle(Item);
		//Update UI
		OnInventoryUpdated.Broadcast(OwningActor);
		return true;
	}
	if(Item && Item->StackCount > 0)
	{
		--Item->StackCount;
		OnInventoryUpdated.Broadcast(OwningActor);
		return true;
	}
	return false;
}

void UMNRInventoryComponent::ClientAddItem_Implementation(UMNRItems* Item, AActor* OwningActor)
{
	AddItem(Item, OwningActor);
}

void UMNRInventoryComponent::ClientRemoveItem_Implementation(UMNRItems* Item, AActor* OwningActor)
{
	RemoveItem(Item, OwningActor);
}

void UMNRInventoryComponent::ServerAddItem_Implementation(UMNRItems* Item, AActor* OwningActor)
{
	AddItem(Item, OwningActor);
}

void UMNRInventoryComponent::ServerRemoveItem_Implementation(UMNRItems* Item, AActor* OwningActor)
{
	RemoveItem(Item, OwningActor);
}

void UMNRInventoryComponent::OnAddItem_Implementation(UMNRItems* Item, AActor* OwningActor)
{
	if (Items.Num() >= Capaticy || !Item)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is null or %c: is greater than %c"), *GetNameSafe(Item), Items.Num(), Capaticy);
		return;
	}
	bool bIsNewItem = true;

	for (auto& ItemVar : Items)
	{
		if (ItemVar->ItemClass.GetDefaultObject() == Item->ItemClass.GetDefaultObject())
		{
			++ItemVar->StackCount;
			bIsNewItem = false;
			break;
		}
	}
	if (bIsNewItem)
	{
		Item->OwningInventory = this;
		Item->World = GetWorld();
		Items.Add(Item);
		//Update UI
		OnInventoryUpdated.Broadcast(OwningActor);
	}
	else
	{
		OnInventoryUpdated.Broadcast(OwningActor);
	}
}

void UMNRInventoryComponent::OnRemoveItem_Implementation(UMNRItems* Item, AActor* OwningActor)
{
	if (Item->StackCount < 1)
	{
		Item->OwningInventory = nullptr;
		Item->World = nullptr;
		Items.RemoveSingle(Item);
		//Update UI
		OnInventoryUpdated.Broadcast(OwningActor);
	}
	if (Item && Item->StackCount > 0)
	{
		Item->StackCount--;
		OnInventoryUpdated.Broadcast(OwningActor);
	}
}

void UMNRInventoryComponent::MulticastAddItem_Implementation(UMNRItems* Item, AActor* OwningActor)
{
	AddItem(Item, OwningActor);
}

void UMNRInventoryComponent::MulticastRemoveItem_Implementation(UMNRItems* Item, AActor* OwningActor)
{
	RemoveItem(Item, OwningActor);
}
