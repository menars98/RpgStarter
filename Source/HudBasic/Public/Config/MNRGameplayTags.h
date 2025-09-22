// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;
/**
 * 
 */
struct FMNRGameplayTags
{
public:

	static const FMNRGameplayTags& Get() { return GameplayTags; }

	static void InitializeNativeTags();

	//Input Tags
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look_Mouse;
	FGameplayTag InputTag_Jump;
	FGameplayTag InputTag_Fire;
	FGameplayTag InputTag_PrimaryInteract;
	FGameplayTag InputTag_Action1;
	FGameplayTag Equipment_Slot_Head;
	FGameplayTag Equipment_Slot_Right_Weapon;
	FGameplayTag Equipment_Slot_Left_Weapon;

protected:

	//Registers all of the tags with the GameplayTags Manager
	void AddAllTags(UGameplayTagsManager& Manager);

	//Helper function used by AddAllTags to register a single tag with the GameplayTags Manager
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);

private:

	static FMNRGameplayTags GameplayTags;
};