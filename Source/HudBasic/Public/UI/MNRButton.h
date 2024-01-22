// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MNRButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMNROnClickedSignature, class UMNRButton*, Button);

/**
 * 
 */
UCLASS()
class HUDBASIC_API UMNRButton : public UUserWidget
{
	GENERATED_BODY()

public:
	// Bind a function with the signature "void OnClicked(UBUIUWButton* Button);
	FMNROnClickedSignature OnClickedDelegate;

protected:
	virtual void NativeConstruct() override;

	// In the Blueprint subclass, make sure you create aButton called "MainButton"
	UPROPERTY(meta = (BindWidget))
	class UButton* MainButton;

	UFUNCTION()
	void OnButtonClicked();
};
