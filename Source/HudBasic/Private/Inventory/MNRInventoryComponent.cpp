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
		AddItem(Item);
	}
	
}

bool UMNRInventoryComponent::AddItem(UMNRItems* Item)
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
		OnInventoryUpdated.Broadcast();
	}
	else
	{
		OnInventoryUpdated.Broadcast();
	}
	return true;
}

bool UMNRInventoryComponent::RemoveItem(UMNRItems* Item)
{
	if(Item->StackCount <1)
	{
		Item->OwningInventory = nullptr;
		Item->World = nullptr;
		Items.RemoveSingle(Item);
		//Update UI
		OnInventoryUpdated.Broadcast();
		return true;
	}
	if(Item && Item->StackCount > 0)
	{
		Item->StackCount--;
		OnInventoryUpdated.Broadcast();
		return true;
	}
	return false;
}

void UMNRInventoryComponent::OnAddItem_Implementation(UMNRItems* Item)
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
		OnInventoryUpdated.Broadcast();
	}
	else
	{
		OnInventoryUpdated.Broadcast();
	}
}

void UMNRInventoryComponent::OnRemoveItem_Implementation(UMNRItems* Item)
{
	if (Item->StackCount < 1)
	{
		Item->OwningInventory = nullptr;
		Item->World = nullptr;
		Items.RemoveSingle(Item);
		//Update UI
		OnInventoryUpdated.Broadcast();
	}
	if (Item && Item->StackCount > 0)
	{
		Item->StackCount--;
		OnInventoryUpdated.Broadcast();
	}
}

/*void UMNRInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMNRInventoryComponent, Items);
}*/

/*bool UMNRInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	return Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
}*/
