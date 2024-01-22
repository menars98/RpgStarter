// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MNRButton.h"

#include "Components/Button.h"

void UMNRButton::NativeConstruct()
{
	Super::NativeConstruct();

	MainButton->OnClicked.AddUniqueDynamic(this, &ThisClass::UMNRButton::OnButtonClicked);
}

void UMNRButton::OnButtonClicked()
{

}
