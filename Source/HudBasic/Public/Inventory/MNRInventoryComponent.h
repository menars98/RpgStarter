// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/MNRItems.h"
#include "Components/ActorComponent.h"
#include "MNRInventoryComponent.generated.h"

struct FItemData;
class UMNRItems;

/*Blueprints will bind to this to update UI*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdated, AActor*, OwningActor);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HUDBASIC_API UMNRInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMNRInventoryComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void TryAddItem(TSubclassOf<UMNRItems> ItemClass, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void TryRemoveItem(TSubclassOf<UMNRItems> ItemClass, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void TryRemoveItemInstance(UMNRItems* ItemInstance);

protected:

	bool RemoveItemInstance_Internal(UMNRItems* ItemInstance);

	// This is the actual insertion logic that runs ONLY ON THE SERVER.
	bool AddItem_Internal(TSubclassOf<UMNRItems> ItemClass, int32 StackCount);

	bool RemoveItem_Internal(TSubclassOf<UMNRItems> ItemClass, int32 StackCount);

	// RPC that receives the “add item” request from the client.
	UFUNCTION(Server, Reliable)
	void Server_TryAddItem(TSubclassOf<UMNRItems> ItemClass, int32 StackCount);

	UFUNCTION(Server, Reliable)
	void Server_TryRemoveItem(TSubclassOf<UMNRItems> ItemClass, int32 StackCount);

	UFUNCTION(Server, Reliable)
	void Server_TryRemoveItemInstance(UMNRItems* ItemInstance);

public:

	// Clients call this function when the Items array is updated.
	// This is the best place to update the UI.
	UFUNCTION()
	void OnRep_Items();

	const TArray<UMNRItems*>& GetItems() const { return Items; }

	UPROPERTY(EditDefaultsOnly, Instanced)
	TArray<class UMNRItems*> DefaultItems;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 Capacity;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	//ReplicatedUsing = OnRep_ItemChanged
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	TArray<class UMNRItems*> Items;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    UMNRItems* FindItemByClass(TSubclassOf<UMNRItems> ItemClass) const
    {
        for (UMNRItems* Item : Items)
        {
            if (Item && Item->IsA(ItemClass))
            {
                return Item;
            }
        }
        return nullptr;
    }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	

	//virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
};

