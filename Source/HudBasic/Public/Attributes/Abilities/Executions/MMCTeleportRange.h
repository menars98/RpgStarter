// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMCTeleportRange.generated.h"

/**
 * 
 */
UCLASS()
class HUDBASIC_API UMMCTeleportRange : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
	UMMCTeleportRange();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

};
