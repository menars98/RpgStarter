// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MNRItems.generated.h"

class AMNRItemActor;
class AMNRHeroCharacter;


/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class HUDBASIC_API UMNRItems : public UObject
{
	GENERATED_BODY()

public:

	UMNRItems();

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Items")
	TSubclassOf<UMNRItems> ItemClass;

	UPROPERTY(Transient)
	class UWorld* World;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Items")
	int32 StackCount;

	/*The text for using the item.(Equip,eat etc.)*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Items")
	FText UseActionText;

	/*The mesh to display for  this items pickup*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Items")
	class UStaticMesh* PickupMesh;

	/*Thumbnail for item*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Items")
	class UTexture2D* Thumbnail;

	/*Display name for inventory*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Items")
	FText ItemDisplayName;

	/*Optional description for item*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Items", meta = (MultiLine = true))
	FText ItemDescription;

	/*Weight of item*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Items", meta = (ClampMin = 0.0))
	float Weight;

	/*The Inventory that owns this item*/
	UPROPERTY()
	class UMNRInventoryComponent* OwningInventory;

	//UPROPERTY()
	//AActor* InFocus;

	UFUNCTION(BlueprintImplementableEvent)
	void OnUse(AMNRHeroCharacter* Character);

	virtual void Use(AActor* Instigator);

	UFUNCTION(Server, Reliable)
	virtual void ServerUse(AActor* Instigator);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;;

	virtual class UWorld* GetWorld() const override { return World; };

	virtual bool IsSupportedForNetworking() const override
	{
		return true;
	}

public:
	UFUNCTION(BlueprintPure, Category = "Items")
	AActor* GetOwningActor() const{ return GetTypedOuter<AActor>(); }

};
