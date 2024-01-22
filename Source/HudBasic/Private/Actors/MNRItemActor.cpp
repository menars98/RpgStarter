// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/MNRItemActor.h"

#include "Character/MNRHeroCharacter.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "InteractableActors"

// Sets default values
AMNRItemActor::AMNRItemActor():Item()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = MeshComponent;

	bReplicates = true;
}

void AMNRItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

/*bool AMNRItemActor::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	// Single Object
	bWroteSomething |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);

	// Array of Objects
	//bWroteSomething |= Channel->ReplicateSubobjectList(ArrayOfMyObject, *Bunch, *RepFlags);

	return bWroteSomething;	
}*/

/*void AMNRItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicating these properties is optional, but likely useful.
	DOREPLIFETIME(AMNRItemActor, Item);
}*/

void AMNRItemActor::Interact_Implementation(APawn* InstigatorPawn)
{
	if(!ensure(InstigatorPawn))
	{
		return;
	}

	AMNRHeroCharacter* Character = Cast<AMNRHeroCharacter>(InstigatorPawn);
	if (Character)
	{
		UE_LOG(LogTemp, Error, TEXT("Successful your character is:%s"), *GetNameSafe(Character));
		Character->AddItemToInventoryWidget(Item.GetDefaultObject());
		Destroy();
	}
	
}

