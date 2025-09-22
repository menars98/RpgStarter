// Fill out your copyright notice in the Description page of Project Settings.


#include "Config/MNRGameplayTags.h"

#include "GameplayTagsManager.h"
#include "Engine/EngineTypes.h"

FMNRGameplayTags FMNRGameplayTags::GameplayTags;

void FMNRGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();

	GameplayTags.AddAllTags(GameplayTagsManager);

	GameplayTagsManager.DoneAddingNativeTags();
}

void FMNRGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	AddTag(InputTag_Move, "InputTag.Move", "Move input.");
	AddTag(InputTag_Look_Mouse, "InputTag.Look.Mouse", "Look (mouse) input.");
	AddTag(InputTag_Jump, "InputTag.Jump", "Jump input");
	AddTag(InputTag_PrimaryInteract, "InputTag.PrimaryInteract", "Interact");
	AddTag(InputTag_Action1, "InputTag.Ability.Action1", "Action1");
	AddTag(Equipment_Slot_Head, "Equipment.Slot.Head", "Head equipment slot");
	AddTag(Equipment_Slot_Right_Weapon, "Equipment.Slot.Right.Weapon", "Right hand weapon slot");
	AddTag(Equipment_Slot_Left_Weapon, "Equipment.Slot.Left.Weapon", "Left hand weapon slot");
}

void FMNRGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}
