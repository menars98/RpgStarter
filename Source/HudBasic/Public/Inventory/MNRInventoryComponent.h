// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MNRInventoryComponent.generated.h"

struct FItemData;
/*Blueprints will bind to this to update UI*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HUDBASIC_API UMNRInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMNRInventoryComponent();

	virtual void BeginPlay() override;

	bool AddItem(class UMNRItems* Item);
	bool RemoveItem(class UMNRItems* Item);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnAddItem(UMNRItems* Item);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnRemoveItem(UMNRItems* Item);

	UPROPERTY(EditDefaultsOnly, Instanced)
	TArray<class UMNRItems*> DefaultItems;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 Capaticy;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	TArray<class UMNRItems*> Items;

	/*virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;*/
};
