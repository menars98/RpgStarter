// Fill out your copyright notice in the Description page of Project Settings.

#include "Attributes/Abilities/MNRAbilitySystemComponent.h"

void UMNRAbilitySystemComponent::ReceiveDamage(UMNRAbilitySystemComponent* SourceASC, float UnmitigatedDamage,
	float MitigatedDamage)
{
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}
