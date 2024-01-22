// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "MNRAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class HUDBASIC_API UMNRAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:

	UMNRAssetManager();

	// Returns the AssetManager singleton object.
	static UMNRAssetManager& Get();

protected:

	virtual void StartInitialLoading() override;
};
