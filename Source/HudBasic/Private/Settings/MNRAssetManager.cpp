// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/MNRAssetManager.h"
#include "AbilitySystemGlobals.h"
#include "Config/MNRGameplayTags.h"

UMNRAssetManager::UMNRAssetManager()
{
}

UMNRAssetManager& UMNRAssetManager::Get()
{
	check(GEngine);

	UMNRAssetManager* MyAssetManager = Cast<UMNRAssetManager>(GEngine->AssetManager);

	if (MyAssetManager)
	{
		return *MyAssetManager;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be GDAssetManager!"));
		return *NewObject<UMNRAssetManager>();	 // never calls this
	}

	
}

void UMNRAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	//Load Native Tags
	FMNRGameplayTags::InitializeNativeTags();

	UAbilitySystemGlobals::Get().InitGlobalData();
	UE_LOG(LogTemp, Warning, TEXT("Initializing Completed"))
}
