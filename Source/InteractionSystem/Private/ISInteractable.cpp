// Fill out your copyright notice in the Description page of Project Settings.


#include "ISInteractable.h"

#include "ISCharacterInteraction.h"

void AISInteractable::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(InteractableTags);
}

// Sets default values
AISInteractable::AISInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

bool AISInteractable::TryDrop(IISCharacterInteraction*pCharacter)
{
	if(!IsGrabbed())
	{
		return false;
	}
	
	RemoveInteractableTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.Grabbed"))));
	return true;
}

void AISInteractable::Interact_Implementation(AActor* pInstigator)
{
	AddInteractableTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.InAction"))));
}

void AISInteractable::GetInHands(IISCharacterInteraction*pCharacter)
{
	AddInteractableTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.Grabbed"))));
}


bool AISInteractable::TryStopInteract(AActor* pInstigator)
{
	if(!IsInInteraction())
	{
		return false;
	}

	RemoveInteractableTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.InAction"))));
	return true;
}

void AISInteractable::SetActive(bool bIsActive)
{
	SetActorEnableCollision(bIsActive);
	SetActorHiddenInGame(!bIsActive);
	SetActorTickEnabled(bIsActive);

	bIsActiveInGame = bIsActive;
}

void AISInteractable::Break_Implementation(AActor* pInstigator)
{
	auto* pCharacterInstigator = Cast<IISCharacterInteraction>(pInstigator);
	if(pCharacterInstigator)
	{
		TryDrop(pCharacterInstigator);	
	}
	
	TryStopInteract(pInstigator);	
	AddInteractableTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.Broken"))));
}

bool AISInteractable::IsGrabbed() const
{
	return HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.Grabbed"))));;
}

bool AISInteractable::IsBroken() const
{
	return HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.Broken"))));
}

bool AISInteractable::IsInInteraction() const
{
	return HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.InAction"))));
}

void AISInteractable::SetIsTargetable(bool _bIsTargetable)
{
	bIsTargetable = _bIsTargetable;
}

/*bool AISInteractable::IsTargetable_Implementation() const
{
	return bIsTargetable && !IsGrabbed();
}*/

UMeshComponent* AISInteractable::GetMeshComponent() const
{
	return nullptr;
}

FGameplayTag AISInteractable::TryUpdateInteraction( const TScriptInterface<IISCharacterInteraction>& pInstigatorCharacter )
{
	return FGameplayTag::EmptyTag;
}

void AISInteractable::AddInteractableTag(const FGameplayTag& _Tag)
{
	
	InteractableTags.AddTag(_Tag);
}

void AISInteractable::RemoveInteractableTag(const FGameplayTag& _Tag)
{
	InteractableTags.RemoveTag(_Tag);
}
