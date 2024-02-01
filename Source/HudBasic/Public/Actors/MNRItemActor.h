// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/MNRGameplayInterface.h"
#include "Inventory/MNRItems.h"
#include "MNRItemActor.generated.h"


class UMNRItems;

UCLASS()
class HUDBASIC_API AMNRItemActor : public AActor, public  IMNRGameplayInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMNRItemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<UMNRItems> Item;


public:

	/*virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;*/

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;


};
