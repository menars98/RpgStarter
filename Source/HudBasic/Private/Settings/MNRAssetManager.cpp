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
	return *MyAssetManager;
}

void UMNRAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	//Load Native Tags
	FMNRGameplayTags::InitializeNativeTags();

	UAbilitySystemGlobals::Get().InitGlobalData();
	UE_LOG(LogTemp, Warning, TEXT("Initializing Completed"))
}
