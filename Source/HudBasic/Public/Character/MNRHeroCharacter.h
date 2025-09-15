// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MNRBaseCharacter.h"
#include "GameplayAbilitySpecHandle.h"
#include <ActiveGameplayEffectHandle.h>
#include "MNRHeroCharacter.generated.h"



struct FItemData;
class UCameraComponent;
class USpringArmComponent;
class UMNREquipmentItem;

USTRUCT(BlueprintType)
struct FStartingItem
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UMNRItems> ItemClass;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (ClampMin = 1))
	int32 StackCount = 1;
};

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

	virtual FVector GetPawnViewLocation() const override;

	// ---Save/Load--- //
	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
	class UMNRSaveGame* GenerateSaveData() const;

	UFUNCTION(BlueprintCallable, Category = "SaveLoad")
	void LoadFromSaveData(const UMNRSaveGame* SaveData);

	// ---Save/Load END--- //
protected:

	TMap<FGameplayTag, TObjectPtr<USkeletalMeshComponent>> EquipmentSlotToMeshComponentMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components | Equipment")
	TObjectPtr<USkeletalMeshComponent> HelmetMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components | Equipment")
	TObjectPtr<USkeletalMeshComponent> ChestMesh;

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

	// Called from both SetupPlayerInputComponent and OnRep_PlayerState because of a potential race condition where the PlayerController might
	// call ClientRestart which calls SetupPlayerInputComponent before the PlayerState is repped to the client so the PlayerState would be null in SetupPlayerInputComponent.
	// Conversely, the PlayerState might be repped before the PlayerController calls ClientRestart so the Actor's InputComponent would be null in OnRep_PlayerState.
	void BindASCInput();
	
	void ASCInputPressed(EMNRAbilityInputID InputID);
	void ASCInputReleased(EMNRAbilityInputID InputID);

	bool ASCInputBound = false;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TArray<FStartingItem> DefaultItems;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UMNRInputConfig* InputActions;

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> EquippedItemAbilityHandles;

	UPROPERTY()
	TMap<FGameplayTag, FActiveGameplayEffectHandle> EquippedEffectHandles;

public:

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	class UMNRInventoryComponent* GetInventoryComponent() const;

	
	void InitializeEquipmentSlotMap();
	// Creates and initializes the floating status bar for heroes.
	// Safe to call many times because it checks to make sure it only executes once.
	UFUNCTION()
	void InitializeFloatingStatusBar();

	//@TODO We can expose it to blueprint if we want to add ability from blueprint
	UFUNCTION(BlueprintCallable, Category = "Items")
	void GrantItemAbilities(const UMNRItems* ItemData);

	UFUNCTION(BlueprintCallable, Category = "Equipment|Abilities")
	void RemoveItemAbilities();

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void EquipItem(UMNREquipmentItem* ItemToEquip);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void UnequipItemFromSlot(const FGameplayTag& SlotTag);

	UFUNCTION(BlueprintCallable, Category = "Items")
	void UseItem(class UMNRItems* Item);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerUseItem(class UMNRItems* Item);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulticastUseItem(class UMNRItems* Item);

	// Client only
	virtual void OnRep_PlayerState() override;

	/** Handles Jumping */
	void Input_Jump();

	/** @TODO we can add fire later */
	void Input_Fire(const FInputActionValue& InputActionValue);

	/** Called for movement input */
	void Input_Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Input_Look(const FInputActionValue& Value);

	void PrimaryInteract();

	//UFUNCTION(BlueprintImplementableEvent, Category = "MNR")
	//void AddItemToInventoryWidget(UMNRItems* Item);

};
