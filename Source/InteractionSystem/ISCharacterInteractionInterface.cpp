// 


#include "ISCharacterInteractionInterface.h"

#include "MInteractable.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"


void IISCharacterInteractionInterface::BeginInteraction()
{
	UPhysicsConstraintComponent* pPhysicsConstraintComponent = GetPhysicsConstraintComponent();
	if(!ensure(IsValid(pPhysicsConstraintComponent))){ return; }
	
	pPhysicsConstraintComponent->OnConstraintBroken.AddDynamic(this, &IISCharacterInteractionInterface::OnPhysicsConstraintBroken);
}


void IISCharacterInteractionInterface::OnPhysicsConstraintBroken(int32 _ConstraintIndex)
{
	//TODO: something on broen
}


void IISCharacterInteractionInterface::UpdateInteraction(float _DeltaTime)
{
	UpdateHandle(_DeltaTime);
	UpdateConstraints(_DeltaTime);
}

void IISCharacterInteractionInterface::AddActiveItem(AMInteractable* pItem)
{
	ActiveItems.Add(pItem);
}

void IISCharacterInteractionInterface::RemoveActiveItem(AMInteractable* pItem)
{
	if(ActiveItems.Contains(pItem))
	{
		ActiveItems.Remove(pItem);
	}
}



void IISCharacterInteractionInterface::UpdateHandle(float _DeltaTime)
{
	UPhysicsHandleComponent* pHandleComp = GetPhysicsHandleComponent();
	if(!IsValid(pHandleComp))
	{
		return;
	}

	UPrimitiveComponent* pGrabbedComponent = pHandleComp->GetGrabbedComponent();
	if(!IsValid(pGrabbedComponent))
	{
		return;
	}

	AMInteractable* pGrabbedActor = Cast<AMInteractable>(pGrabbedComponent->GetAttachmentRootActor());
	if(!IsValid(pGrabbedActor))
	{
		return;
	}

	const FGameplayTag UpdateResult = pGrabbedActor->TryUpdateInteraction(this);
	if(UpdateResult == FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.Stop.Drop"))))
	{
		//TODO: WTF
		pHandleComp->ReleaseComponent();
		//CancelGrabbingAbility();
	}
}

void IISCharacterInteractionInterface::UpdateConstraints(float _DeltaTime)
{
	UPhysicsConstraintComponent* pPhysicsConstraintComp = GetPhysicsConstraintComponent();
	if(!IsValid(pPhysicsConstraintComp))
	{
		return;
	}
	AMInteractable* pGrabbedActor = Cast<AMInteractable>(pPhysicsConstraintComp->ConstraintActor2 == this ? pPhysicsConstraintComp->ConstraintActor1 : pPhysicsConstraintComp->ConstraintActor2);
	if(!IsValid(pGrabbedActor))
	{
		return;
	}

	const FGameplayTag UpdateResult = pGrabbedActor->TryUpdateInteraction(this);
	if(UpdateResult == FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.Stop.Drop"))))
	{
		pHandleComp->
		//TODO: WTF
		//CancelGrabbingAbility();
	}
}




