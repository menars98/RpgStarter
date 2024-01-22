// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "MNRDamageExecCalculation.generated.h"

/**
 * 
 */
UCLASS()
class HUDBASIC_API UMNRDamageExecCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

	UMNRDamageExecCalculation();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
