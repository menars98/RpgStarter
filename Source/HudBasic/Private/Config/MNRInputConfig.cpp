// Fill out your copyright notice in the Description page of Project Settings.


#include "Config/MNRInputConfig.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"

const UInputAction* UMNRInputConfig::FindInputActionForTag(const FGameplayTag& InputTag) const
{
	for (const FTaggedInputAction& TaggedInputAction : TaggedInputActions)
	{
		if (TaggedInputAction.InputAction && TaggedInputAction.InputTag == InputTag)
		{
			return TaggedInputAction.InputAction;
		}
	}

	return nullptr;
}
