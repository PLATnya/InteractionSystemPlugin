// 


#include "MInteractable.h"

#include "ISCharacterInteractionInterface.h"


void AMInteractable::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(InteractableTags);
}

// Sets default values
AMInteractable::AMInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

bool AMInteractable::TryDrop_Implementation(IISCharacterInteractionInterface* pCharacter)
{
	if(!IsGrabbed())
	{
		return false;
	}
	
	RemoveInteractableTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.Grabbed"))));
	return true;
}

void AMInteractable::GetInHands_Implementation(IISCharacterInteractionInterface* pCharacter)
{
	AddInteractableTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.Grabbed"))));
}

void AMInteractable::Interact_Implementation(AActor* pInstigator)
{
	AddInteractableTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.InAction"))));
}

bool AMInteractable::TryStopInteract_Implementation(AActor* pInstigator)
{
	if(!IsInInteraction())
	{
		return false;
	}

	RemoveInteractableTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.InAction"))));
	return true;
}

void AMInteractable::SetActive(bool bIsActive)
{
	SetActorEnableCollision(bIsActive);
	SetActorHiddenInGame(!bIsActive);
	SetActorTickEnabled(bIsActive);

	bIsActiveInGame = bIsActive;
}

void AMInteractable::Break_Implementation(AActor* pInstigator)
{
	auto* pCharacterInstigator = Cast<IISCharacterInteractionInterface>(pInstigator);
	if(pCharacterInstigator)
	{
		TryDrop(pCharacterInstigator);	
	}
	
	TryStopInteract(pInstigator);	
	AddInteractableTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.Broken"))));
}

bool AMInteractable::IsGrabbed() const
{
	return HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.Grabbed"))));;
}

bool AMInteractable::IsBroken() const
{
	return HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.Broken"))));
}

bool AMInteractable::IsInInteraction() const
{
	return HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.InAction"))));
}

void AMInteractable::SetIsTargetable(bool _bIsTargetable)
{
	bIsTargetable = _bIsTargetable;
}

/*bool AMInteractable::IsTargetable_Implementation() const
{
	return bIsTargetable && !IsGrabbed();
}*/

UMeshComponent* AMInteractable::GetMeshComponent_Implementation() const
{
	return nullptr;
}

FGameplayTag AMInteractable::TryUpdateInteraction( IISCharacterInteractionInterface* pInstigatorCharacter )
{
	return FGameplayTag::EmptyTag;
}

void AMInteractable::AddInteractableTag(const FGameplayTag& _Tag)
{
	
	InteractableTags.AddTag(_Tag);
}

void AMInteractable::RemoveInteractableTag(const FGameplayTag& _Tag)
{
	InteractableTags.RemoveTag(_Tag);
}
