// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MNRHeroCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "MNRGameModeBase.h"
#include "Attributes/Abilities/MNRAbilitySystemComponent.h"
#include "Attributes/MNRAttributeSetBase.h"
#include "GameFramework/PlayerInput.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Config/MNRGameplayTags.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/MNREnhancedInputComponent.h"
#include "Interaction/MNRInteractionComponent.h"
#include "Inventory/MNRInventoryComponent.h"
#include "Inventory/MNRItems.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MNRPlayerController.h"
#include "Player/MNRPlayerState.h"
#include "UI/MNRFloatingStatusBarWidget.h"
#include <MNRGameInstance.h>
#include "SaveLoad/MNRSaveGame.h"
#include "Inventory/MNREquipmentItem.h"


AMNRHeroCharacter::AMNRHeroCharacter(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetupAttachment(RootComponent);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(FName("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);

	GunComponent = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Gun"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	// Makes sure that the animations play on the Server so that we can use bone and socket transforms
	// to do things like spawning projectiles and other FX.
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));

	//When we make hero class move to hero class below things
	UIFloatingStatusBarComponent = CreateDefaultSubobject<UWidgetComponent>(FName("UIFloatingStatusBarComponent"));
	//UIFloatingStatusBarComponent->SetupAttachment();
	UIFloatingStatusBarComponent->SetRelativeLocation(FVector(0, 0, 120));
	UIFloatingStatusBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	UIFloatingStatusBarComponent->SetDrawSize(FVector2D(500, 500));

	UIFloatingStatusBarClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Game/GASDocumentation/UI/UI_FloatingStatusBar_Hero.UI_FloatingStatusBar_Hero_C"));
	if (!UIFloatingStatusBarClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Failed to find UIFloatingStatusBarClass. If it was moved, please update the reference location in C++."), *FString(__FUNCTION__));
	}

	InventoryComponent = CreateDefaultSubobject<UMNRInventoryComponent>("Inventory");
	InventoryComponent->Capacity = 20;
	InventoryComponent->SetIsReplicated(true);

	InteractionComp = CreateDefaultSubobject<UMNRInteractionComponent>("InteractionComp");

	HelmetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HelmetMesh"));
	HelmetMesh->SetupAttachment(GetMesh()); 
	HelmetMesh->SetMasterPoseComponent(GetMesh()); 

	ChestMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ChestMesh"));
	ChestMesh->SetupAttachment(GetMesh());
	ChestMesh->SetMasterPoseComponent(GetMesh()); 
}

void AMNRHeroCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Only needed for Heroes placed in world and when the player is the Server.
	// On respawn, they are set up in PossessedBy.
	// When the player a client, the floating status bars are all set up in OnRep_PlayerState.
	InitializeFloatingStatusBar();
	InitializeEquipmentSlotMap();

	StartingCameraBoomArmLength = CameraBoom->TargetArmLength;
	StartingCameraBoomLocation = CameraBoom->GetRelativeLocation();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
}

void AMNRHeroCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GunComponent && GetMesh())
	{
		GunComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GunSocket"));
	}
}

FVector AMNRHeroCharacter::GetPawnViewLocation() const
{
	return FollowCamera->GetComponentLocation();
}

void AMNRHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AMNRPlayerState* PS = GetPlayerState<AMNRPlayerState>();
	if (PS)
	{
		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		AbilitySystemComponent = Cast<UMNRAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		if (HasAuthority())
		{
			UMNRGameInstance* GameInstance = Cast<UMNRGameInstance>(GetGameInstance());

			if (GameInstance && GameInstance->PlayerSaveData)
			{
				// LOADING STATUS: Load the entire status with a single function.
				LoadFromSaveData(GameInstance->PlayerSaveData);
				GameInstance->PlayerSaveData = nullptr;
			}
			else
			{
				// NEW GAME STATUS: Give starting items.
				if (InventoryComponent)
				{
					for (const FStartingItem& StartupItem : DefaultItems)
					{
						if (StartupItem.ItemClass)
						{
							InventoryComponent->TryAddItem(StartupItem.ItemClass, StartupItem.StackCount);
						}
					}
				}
			}

			// Set the AttributeSetBase for convenience attribute functions
			AttributeSetBase = TWeakObjectPtr<UMNRAttributeSetBase>(PS->GetAttributeSetBase());

			// If we handle players disconnecting and rejoining in the future, we'll have to change this so that possession from rejoining doesn't reset attributes.
			// For now assume possession = spawn/respawn.
			InitializeAttributes();

			// Respawn specific things that won't affect first possession.

			// Forcibly set the DeadTag count to 0
			AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

			// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
			SetHealth(GetMaxHealth());
			SetMana(GetMaxMana());
			SetStamina(GetMaxStamina());

			// End respawn specific things
			AddStartupEffects();

			AddCharacterAbilities();

			AMNRPlayerController* PC = Cast<AMNRPlayerController>(GetController());
			if (PC)
			{
				PC->CreateHUD();
			}

			InitializeFloatingStatusBar();
		}
	}
}
// Called to bind functionality to input
void AMNRHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	const APlayerController* PC = GetController<APlayerController>();
	const ULocalPlayer* LP = PC->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* SubSystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(SubSystem);

	SubSystem->ClearAllMappings();

	SubSystem->AddMappingContext(DefaultMappingContext, 0);

	UMNREnhancedInputComponent* MyEnhancedInputComponent = Cast<UMNREnhancedInputComponent>(PlayerInputComponent);

	//Make sure to set your input component class in the InputSettings->DefaultClasses
	check(MyEnhancedInputComponent);

	const FMNRGameplayTags GameplayTags = FMNRGameplayTags::Get();

	MyEnhancedInputComponent->BindActionByTag(InputActions, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &AMNRHeroCharacter::Input_Move);
	MyEnhancedInputComponent->BindActionByTag(InputActions, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &AMNRHeroCharacter::Input_Look);
	MyEnhancedInputComponent->BindActionByTag(InputActions, GameplayTags.InputTag_Jump, ETriggerEvent::Triggered, this, &AMNRHeroCharacter::Input_Jump);
	MyEnhancedInputComponent->BindActionByTag(InputActions, GameplayTags.InputTag_PrimaryInteract, ETriggerEvent::Triggered, this, &AMNRHeroCharacter::PrimaryInteract);
	//MyEnhancedInputComponent->BindActionByTag(InputActions, GameplayTags.InputTag_Action1, ETriggerEvent::Triggered, this, &AMNRHeroCharacter::Input_Fire);

	// Bind player input to the AbilitySystemComponent. Also called in OnRep_PlayerState because of a potential race condition.
	BindASCInput();
}

void AMNRHeroCharacter::Input_Jump()
{
	Jump();
}

void AMNRHeroCharacter::Input_Fire(const FInputActionValue& InputActionValue)
{
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(EMNRAbilityInputID::Action1));
	}
}

void AMNRHeroCharacter::Input_Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMNRHeroCharacter::Input_Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMNRHeroCharacter::PrimaryInteract()
{
	if(InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

UMNRInventoryComponent* AMNRHeroCharacter::GetInventoryComponent() const
{
	return InventoryComponent;
}

void AMNRHeroCharacter::InitializeEquipmentSlotMap()
{
	// Haritanýn zaten dolu olmadýðýný kontrol et.
	if (EquipmentSlotToMeshComponentMap.Num() > 0)
	{
		return;
	}

	// Her bir slot etiketi için, ilgili mesh component'i haritaya ekle.
	// FMNRGameplayTags::Get() senin etiketlerini merkezi olarak tutan singleton'ýn olduðunu varsayar.
	// Eðer böyle bir yapýn yoksa, FGameplayTag::RequestGameplayTag(FName("...")) kullanabilirsin.
	const FMNRGameplayTags GameplayTags = FMNRGameplayTags::Get();

	EquipmentSlotToMeshComponentMap.Add(GameplayTags.Equipment_Slot_Head, HelmetMesh);
	EquipmentSlotToMeshComponentMap.Add(FGameplayTag::RequestGameplayTag(FName("Equipment.Slot.Chest")), ChestMesh);
	//EquipmentSlotToMeshComponentMap.Add(FGameplayTag::RequestGameplayTag(FName("Equipment.Slot.Legs")), LegsMesh);
	//EquipmentSlotToMeshComponentMap.Add(FGameplayTag::RequestGameplayTag(FName("Equipment.Slot.Feet")), FeetMesh);
}

void AMNRHeroCharacter::InitializeFloatingStatusBar()
{
	// Only create once
	if (UIFloatingStatusBar || !AbilitySystemComponent.IsValid())
	{
		return;
	}

	// Setup UI for Locally Owned Players only, not AI or the server's copy of the PlayerControllers
	AMNRPlayerController* PC = Cast<AMNRPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC && PC->IsLocalPlayerController())
	{
		if (UIFloatingStatusBarClass)
		{
			UIFloatingStatusBar = CreateWidget<UMNRFloatingStatusBarWidget>(PC, UIFloatingStatusBarClass);
			if (UIFloatingStatusBar && UIFloatingStatusBarComponent)
			{
				UIFloatingStatusBarComponent->SetWidget(UIFloatingStatusBar);

				// Setup the floating status bar
				UIFloatingStatusBar->SetHealthPercentage(GetHealth() / GetMaxHealth());
				UIFloatingStatusBar->SetManaPercentage(GetMana() / GetMaxMana());
			}
		}
	}
}

void AMNRHeroCharacter::GrantItemAbilities(const UMNRItems* ItemData)
{
	// Önce varsa eski item'ýn yeteneklerini temizle.
	RemoveItemAbilities();


	for (const TSubclassOf<UGameplayAbility>& AbilityClass : ItemData->GrantedAbilities)
	{
		if (AbilityClass)
		{
			// For now, let's assume we've assigned all item abilities to Action1.
			// In a more advanced system, ItemData might also contain information about which ability goes to which InputID.
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1, static_cast<int32>(EMNRAbilityInputID::Action1), this);

			FGameplayAbilitySpecHandle GrantedHandle = AbilitySystemComponent->GiveAbility(AbilitySpec);
			EquippedItemAbilityHandles.Add(GrantedHandle);
		}
	}
}

void AMNRHeroCharacter::RemoveItemAbilities()
{
	//Remove from ASC
	for (const FGameplayAbilitySpecHandle& Handle : EquippedItemAbilityHandles)
	{
		AbilitySystemComponent->ClearAbility(Handle);
	}

	// Clear Handle
	EquippedItemAbilityHandles.Empty();
}

void AMNRHeroCharacter::EquipItem(UMNREquipmentItem* ItemToEquip)
{
	if (!AbilitySystemComponent.IsValid() || !ItemToEquip) return;

	// 1. First, remove the current item from this slot.
	UnequipItemFromSlot(ItemToEquip->EquipmentSlotTag);

	// 2. Apply the GameplayEffect for the new equipment.
	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(ItemToEquip);
	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(ItemToEquip->EquipmentEffect, 1.0f, ContextHandle);
	if (SpecHandle.IsValid())
	{
		FActiveGameplayEffectHandle EffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		// Save the handle in a Map so you can remove this effect later.
		EquippedEffectHandles.Add(ItemToEquip->EquipmentSlotTag, EffectHandle);
	}


	if (TObjectPtr<USkeletalMeshComponent>* TargetMeshComponentPtr = EquipmentSlotToMeshComponentMap.Find(ItemToEquip->EquipmentSlotTag))
	{
		// 2. Replace the found component's mesh with the item's mesh.
		if (USkeletalMeshComponent* TargetMeshComponent = *TargetMeshComponentPtr)
		{
			TargetMeshComponent->SetSkeletalMesh(ItemToEquip->EquipmentMesh);
		}
	}
}

void AMNRHeroCharacter::UnequipItemFromSlot(const FGameplayTag& SlotTag)
{
	if (!AbilitySystemComponent.IsValid() || !SlotTag.IsValid()) return;

	// Is there a registered effect for this slot?
	if (EquippedEffectHandles.Contains(SlotTag))
	{
		//Remove the effect.This will restore the stat bonuses and the tag.
		AbilitySystemComponent->RemoveActiveGameplayEffect(EquippedEffectHandles[SlotTag]);
		EquippedEffectHandles.Remove(SlotTag);

		if (TObjectPtr<USkeletalMeshComponent>* TargetMeshComponentPtr = EquipmentSlotToMeshComponentMap.Find(SlotTag))
		{
			// 2. Clear the mesh of the found component.
			if (USkeletalMeshComponent* TargetMeshComponent = *TargetMeshComponentPtr)
			{
				TargetMeshComponent->SetSkeletalMesh(nullptr);
			}
		}
	}
}

void AMNRHeroCharacter::UseItem(UMNRItems* Item)
{
	if (Item)
	{
		// Call ServerUseItem, which triggers Multicast.
		// This ensures the action always starts from the Server.
		ServerUseItem(Item);
	}
}

void AMNRHeroCharacter::ServerUseItem_Implementation(UMNRItems* Item)
{
	// The server determines that the action is valid and notifies all clients.
	MulticastUseItem(Item);
}

void AMNRHeroCharacter::MulticastUseItem_Implementation(UMNRItems* Item)
{
	if (Item)
    {
        Item->Use(this); 
		Item->OnUse(this);
    };
}

void AMNRHeroCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AMNRPlayerState* PS = GetPlayerState<AMNRPlayerState>();
	if (PS)
	{
		// Set the ASC for clients. Server does this in PossessedBy.
		AbilitySystemComponent = Cast<UMNRAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSetBase = TWeakObjectPtr<UMNRAttributeSetBase>(PS->GetAttributeSetBase());

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that posession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		InitializeAttributes();

		AMNRPlayerController* PC = Cast<AMNRPlayerController>(GetController());
		if (PC)
		{
			PC->CreateHUD();
		}

		BindASCInput();


		// Simulated on proxies don't have their PlayerStates yet when BeginPlay is called so we call it again here
		InitializeFloatingStatusBar();


		// Respawn specific things that won't affect first possession.

		// Forcibly set the DeadTag count to 0
		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());
		SetStamina(GetMaxStamina());
	}
}

USpringArmComponent* AMNRHeroCharacter::GetCameraBoom()
{
	return CameraBoom;
}

UCameraComponent* AMNRHeroCharacter::GetFollowCamera()
{
	return FollowCamera;
}

float AMNRHeroCharacter::GetStartingCameraBoomArmLength()
{
	return StartingCameraBoomArmLength;
}

FVector AMNRHeroCharacter::GetStartingCameraBoomLocation()
{
	return StartingCameraBoomLocation;
}

UMNRFloatingStatusBarWidget* AMNRHeroCharacter::GetFloatingStatusBar()
{
	return UIFloatingStatusBar;
}

USkeletalMeshComponent* AMNRHeroCharacter::GetGunComponent() const
{
	return GunComponent;
}

void AMNRHeroCharacter::FinishDying()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		AMNRGameModeBase* GM = Cast<AMNRGameModeBase>(GetWorld()->GetAuthGameMode());

		if (GM)
		{
			GM->HeroDied(GetController());
		}
	}

	Super::FinishDying();
}

void AMNRHeroCharacter::BindASCInput()
{
	if (!ASCInputBound && AbilitySystemComponent.IsValid() && InputComponent && InputActions)
	{
		UEnhancedInputComponent* MyEnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
		if (MyEnhancedInputComponent)
		{
			for (const FTaggedInputAction& Action : InputActions->AbilityInputActions)
			{
				if (Action.InputAction && Action.InputID != EMNRAbilityInputID::None)
				{
					//Useful for skills involving the style of releasing and holding flame bursts
					// Link the ‘Triggered’ (press) moment of the action.
					MyEnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Triggered, this, &AMNRHeroCharacter::ASCInputPressed, Action.InputID);

					// Link the moment the action is ‘Completed’ (abandoned).
					MyEnhancedInputComponent->BindAction(Action.InputAction, ETriggerEvent::Completed, this, &AMNRHeroCharacter::ASCInputReleased, Action.InputID);
				}
			}
		}

		ASCInputBound = true; // Mark to prevent reconnection.
	}
}

void AMNRHeroCharacter::ASCInputPressed(EMNRAbilityInputID InputID)
{
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(InputID));
	}
}

void AMNRHeroCharacter::ASCInputReleased(EMNRAbilityInputID InputID)
{
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(InputID));
	}
}

UMNRSaveGame* AMNRHeroCharacter::GenerateSaveData() const
{
	// 1. Create a new SaveGame object.
	UMNRSaveGame* SaveData = NewObject<UMNRSaveGame>();

	// 2. Save Inventory
	if (InventoryComponent)
	{
		for (const UMNRItems* Item : InventoryComponent->GetItems())
		{
			if (Item)
			{
				FInventorySlotData SlotData;
				SlotData.ItemClass = Item->GetClass();
				SlotData.StackCount = Item->StackCount;
				SaveData->SavedInventoryItems.Add(SlotData);
			}
		}
	}
	// 3. Save Equipped Items
	// This assumes your character has functions like GetEquippedWeapon() and GetEquippedHelmet().
	/*if (GetEquippedWeapon())
	{
		SaveData->SavedEquippedItems.Add(EEquipmentSlot::Weapon, GetEquippedWeapon()->GetClass());
	}
	if (GetEquippedHelmet())
	{
		SaveData->SavedEquippedItems.Add(EEquipmentSlot::Helmet, GetEquippedHelmet()->GetClass());
	}*/
	// ... diðer slotlar ...

	// 4. Save the Health Value
	if (AttributeSetBase.IsValid())
	{
		SaveData->SavedHealth = AttributeSetBase->GetHealth();
	}

	return SaveData;
}

void AMNRHeroCharacter::LoadFromSaveData(const UMNRSaveGame* SaveData)
{
	if (!SaveData || !InventoryComponent) return;


	for (const FInventorySlotData& SlotData : SaveData->SavedInventoryItems)
	{
		InventoryComponent->TryAddItem(SlotData.ItemClass, SlotData.StackCount);
	}

	// Note: This should be done AFTER the inventory is loaded!
	for (const TPair<EEquipmentSlot, TSubclassOf<UMNRItems>>& EquippedItemPair : SaveData->SavedEquippedItems)
	{
		UMNRItems* ItemToEquip = InventoryComponent->FindItemByClass(EquippedItemPair.Value);
		if (ItemToEquip)
		{
			UseItem(ItemToEquip); 
		}
	}

	// 3. Load Health Points (It is best to do this with Gameplay Effect)
	if (AttributeSetBase.IsValid() && SaveData->SavedHealth > 0)
	{
		// This is the simplest method, but it can cause replication issues.
		 AttributeSetBase->SetHealth(SaveData->SavedHealth); 
		 // The best method is to implement a GE that sets the value to this.
	}
}