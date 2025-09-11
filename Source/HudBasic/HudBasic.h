// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EMNRAbilityInputID : uint8
{
    None,
    Confirm,    // Onayla (örn: menüde)
    Cancel,     // Ýptal
    Primary,    // Birincil Saldýrý (örn: Sol Týk)
    Secondary,  // Ýkincil Saldýrý (örn: Sað Týk)
    Action1     // F tuþu gibi özel bir eylem
};