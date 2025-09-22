// Fill out your copyright notice in the Description page of Project Settings.


#include "Attributes/Abilities/Executions/MMCTeleportRange.h"

UMMCTeleportRange::UMMCTeleportRange()
{

}

float UMMCTeleportRange::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    // 1. Get ability level
    const float AbilityLevel = Spec.GetLevel();

	// 2. Calculate range based on level
    // @TODO: Use Curve

    if (AbilityLevel == 2.0f)
    {
        return 3000.0f;
    }

    return 2000.0f;
}
