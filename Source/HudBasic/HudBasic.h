// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EMNRAbilityInputID : uint8
{
    None,
    Confirm,    
    Cancel,     
    Primary,    // Primary Attack (e.g., Left Click)
    Secondary,  // Secondary Attack (e.g., Right-Click)
    Action1,    // A special action
    Teleport
};

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
    None        UMETA(DisplayName = "None"),
    Helmet      UMETA(DisplayName = "Helmet"),
    Chest       UMETA(DisplayName = "Chest"),
    Weapon      UMETA(DisplayName = "Weapon")
    // Other slots to be added later (Boots, Gloves, etc.)
};