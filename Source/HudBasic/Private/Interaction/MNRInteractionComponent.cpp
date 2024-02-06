// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/MNRInteractionComponent.h"

#include "Blueprint/UserWidget.h"
#include "Interaction/MNRGameplayInterface.h"
#include "UI/MNRWorldUserWidget.h"
#include "DrawDebugHelpers.h"
#include <Character/MNRHeroCharacter.h>

static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("mnr.InteractionDebugDraw"), false, TEXT("Enable Debug Lines for Interact Component."), ECVF_Cheat);

// Sets default values for this component's properties
UMNRInteractionComponent::UMNRInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TraceRadius = 30.0f;
	TraceDistance = 500.0f;
	CollisionChannel = ECC_WorldDynamic;
}

void UMNRInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APawn* MyPawn = Cast<APawn>(GetOwner());

	//Is Locally Controlled, then run this function.
	if (MyPawn->IsLocallyControlled())
	{
		FindBestInteractable();
	}
}

void UMNRInteractionComponent::FindBestInteractable()
{
	bool bDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);

	AActor* MyOwner = GetOwner();

	AMNRHeroCharacter* MyCharacter = Cast<AMNRHeroCharacter>(MyOwner);

	FVector EyeLocation;
	FRotator EyeRotation;

	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	
	EyeLocation = MyCharacter->GetPawnViewLocation();

	FVector End = EyeLocation + (EyeRotation.Vector() * TraceDistance);

	TArray<FHitResult> Hits;

	FCollisionShape Shape;
	Shape.SetSphere(TraceRadius);

	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);

	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

	//Clear ref before trying to fill
	FocusedActor = nullptr;

	for (FHitResult Hit : Hits)
	{
		if (bDebugDraw)
		{
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, TraceRadius, 32, LineColor, false, 0.0f);
		}

		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			// If it implements hit actor, assign it to focusedactor
			if (HitActor->Implements<UMNRGameplayInterface>())
			{

				FocusedActor = HitActor;
				break;
			}
		}
	}

	if (FocusedActor)
	{
		//If class is not null and instance is not created, create a widget
		if (DefaultWidgetInstance == nullptr && ensure(DefaultWidgetClass))
		{
			DefaultWidgetInstance = CreateWidget<UMNRWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		}
		// Attach instance to focusedactor
		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->AttachedActor = FocusedActor;

			if (!DefaultWidgetInstance->IsInViewport())
			{
				DefaultWidgetInstance->AddToViewport();
			}
		}
	}
	//If there is no focused actor, remove instance
	else
	{
		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->RemoveFromParent();
		}
	}
	if (bDebugDraw)
	{
		DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 0.0f, 0, 2.0f);
	}

}
void UMNRInteractionComponent::PrimaryInteract()
{
	//Call it from serverinteract
	ServerInteract(FocusedActor);
}

void UMNRInteractionComponent::ServerInteract_Implementation(AActor* InFocus)
{
	if (InFocus == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "No Focus Actor to interact");
		return;
	}

	APawn* MyPawn = Cast<APawn>(GetOwner());
	
	IMNRGameplayInterface::Execute_Interact(InFocus, MyPawn);
}

// Called when the game starts
void UMNRInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
}






