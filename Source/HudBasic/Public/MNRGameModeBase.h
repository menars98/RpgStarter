// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MNRGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class HUDBASIC_API AMNRGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMNRGameModeBase();

	void HeroDied(AController* Controller);

protected:

	float RespawnDelay;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character")
	TSubclassOf<class AMNRHeroCharacter> HeroClass;

	UPROPERTY()
	AActor* EnemySpawnPoint;

	virtual void BeginPlay() override;

	void RespawnHero(AController* Controller);
};
