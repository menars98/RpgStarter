// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/MNRItems.h"

#include "Character/MNRHeroCharacter.h"
#include "Net/UnrealNetwork.h"


UMNRItems::UMNRItems()
{
	Weight = 1.0f;
	ItemDisplayName = FText::FromString("Item");
	UseActionText = FText::FromString("Use");
	StackCount = 1;
	ItemClass = StaticClass();
}

void UMNRItems::Use(AMNRHeroCharacter* Character)
{

}


auto UMNRItems::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const -> void
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	if (const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
	{
		BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
	}
}






