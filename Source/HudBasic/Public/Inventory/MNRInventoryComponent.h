// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MNRInventoryComponent.generated.h"

struct FItemData;
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

	bool AddItem(class UMNRItems* Item, AActor* OwningActor);
	bool RemoveItem(class UMNRItems* Item, AActor* OwningActor);

	//@TODO Fix this functions all of them doing same thing
	UFUNCTION(Server, Reliable)
	void ServerAddItem(class UMNRItems* Item, AActor* OwningActor);

	UFUNCTION(Server, Reliable)
	void ServerRemoveItem(class UMNRItems* Item, AActor* OwningActor);

	UFUNCTION(Client, Reliable)
	void ClientAddItem(class UMNRItems* Item, AActor* OwningActor);

	UFUNCTION(Client, Reliable)
	void ClientRemoveItem(class UMNRItems* Item, AActor* OwningActor);

	UFUNCTION(NetMulticast,Reliable)
	void MulticastAddItem(class UMNRItems* Item, AActor* OwningActor);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRemoveItem(class UMNRItems* Item, AActor* OwningActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnAddItem(UMNRItems* Item, AActor* OwningActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRemoveItem(UMNRItems* Item, AActor* OwningActor);

	UPROPERTY(EditDefaultsOnly, Instanced)
	TArray<class UMNRItems*> DefaultItems;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 Capaticy;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;
	//ReplicatedUsing = OnRep_ItemChanged
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	TArray<class UMNRItems*> Items;


	//virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
};
