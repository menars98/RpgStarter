// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MNRBaseCharacter.h"
#include "MNRHeroCharacter.generated.h"

struct FItemData;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class HUDBASIC_API AMNRHeroCharacter : public AMNRBaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMNRHeroCharacter(const class FObjectInitializer& ObjectInitializer);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Only called on the Server. Calls before Server's AcknowledgePossession.
	virtual void PossessedBy(AController* NewController) override;

	class USpringArmComponent* GetCameraBoom();

	class UCameraComponent* GetFollowCamera();

	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|Camera")
	float GetStartingCameraBoomArmLength();

	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|Camera")
	FVector GetStartingCameraBoomLocation();

	class UMNRFloatingStatusBarWidget* GetFloatingStatusBar();

	USkeletalMeshComponent* GetGunComponent() const;

	virtual void FinishDying() override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASDocumentation|Camera")
	float BaseTurnRate = 45.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASDocumentation|Camera")
	float BaseLookUpRate = 45.0f;

	UPROPERTY(BlueprintReadOnly, Category = "GASDocumentation|Camera")
	float StartingCameraBoomArmLength;

	UPROPERTY(BlueprintReadOnly, Category = "GASDocumentation|Camera")
	FVector StartingCameraBoomLocation;

	/* Interaction */
	UPROPERTY(VisibleAnywhere)
	UMNRInteractionComponent* InteractionComp;

	/*Inventory*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	class UMNRInventoryComponent* InventoryComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GASDocumentation|Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GASDocumentation|Camera")
	class UCameraComponent* FollowCamera;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	USkeletalMeshComponent* GunComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	TSubclassOf<class UMNRFloatingStatusBarWidget> UIFloatingStatusBarClass;

	UPROPERTY()
	class UMNRFloatingStatusBarWidget* UIFloatingStatusBar;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI")
	class UWidgetComponent* UIFloatingStatusBarComponent;

	FGameplayTag DeadTag;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UMNRInputConfig* InputActions;

public:

	// Creates and initializes the floating status bar for heroes.
	// Safe to call many times because it checks to make sure it only executes once.
	UFUNCTION()
	void InitializeFloatingStatusBar();

	UFUNCTION(BlueprintCallable, Category = "Items")
	void UseItem(class UMNRItems* Item);

	// Client only
	virtual void OnRep_PlayerState() override;

	/** Handles Jumping */
	void Input_Jump();

	/** @TODO we can add fire later */
	//void Input_Fire(const FInputActionValue& InputActionValue);

	/** Called for movement input */
	void Input_Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Input_Look(const FInputActionValue& Value);

	void PrimaryInteract();

	UFUNCTION(BlueprintImplementableEvent, Category = "MNR")
	void AddItemToInventoryWidget(UMNRItems* Item);
};
