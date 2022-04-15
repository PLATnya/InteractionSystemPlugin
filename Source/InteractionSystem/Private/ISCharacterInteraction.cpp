// Fill out your copyright notice in the Description page of Project Settings.


#include "ISCharacterInteraction.h"

#include "ISInteractable.h"
#include "GameFramework/Character.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"


void IISCharacterInteraction::BeginInteraction()
{
	/*UPhysicsConstraintComponent* pPhysicsConstraintComponent = GetPhysicsConstraintComponent();
	if(!ensure(IsValid(pPhysicsConstraintComponent))){ return; }
	
	pPhysicsConstraintComponent->OnConstraintBroken.AddDynamic(this, &IISCharacterInteraction::OnPhysicsConstraintBroken);*/
	
}


void IISCharacterInteraction::OnPhysicsConstraintBroken(int32 _ConstraintIndex)
{
	//TODO: something on broen
}

const TScriptInterface<IISCharacterInteraction>& IISCharacterInteraction::GetScriptInterface()
{
	if(!ScriptInterface.GetInterface())
	{
		ScriptInterface.SetInterface(this);
	}
	
	return ScriptInterface;
}


void IISCharacterInteraction::UpdateInteraction(float _DeltaTime)
{
	UpdateHandle(_DeltaTime);
	UpdateConstraints(_DeltaTime);
}

/*
void IISCharacterInteraction::AddActiveItem(AISInteractable* pItem)
{
	ActiveItems.Add(pItem);
}

void IISCharacterInteraction::RemoveActiveItem(AISInteractable* pItem)
{
	if(ActiveItems.Contains(pItem))
	{
		ActiveItems.Remove(pItem);
	}
}*/



void IISCharacterInteraction::UpdateHandle(float _DeltaTime)
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

	AISInteractable* pGrabbedActor = Cast<AISInteractable>(pGrabbedComponent->GetAttachmentRootActor());
	if(!IsValid(pGrabbedActor))
	{
		return;
	}

	
	const FGameplayTag UpdateResult = pGrabbedActor->TryUpdateInteraction(GetScriptInterface());
	if(UpdateResult == FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.Stop.Drop"))))
	{
		//TODO: WTF
		pHandleComp->ReleaseComponent();
		//CancelGrabbingAbility();
	}
}

void IISCharacterInteraction::UpdateConstraints(float _DeltaTime)
{
	UPhysicsConstraintComponent* pPhysicsConstraintComp = GetPhysicsConstraintComponent();
	if(!IsValid(pPhysicsConstraintComp))
	{
		return;
	}

	const ACharacter* pCharacter = Cast<ACharacter>(this);
	if(!ensure(IsValid(pCharacter))){ return; }
	
	AISInteractable* pGrabbedActor = Cast<AISInteractable>(pPhysicsConstraintComp->ConstraintActor2 == pCharacter ? pPhysicsConstraintComp->ConstraintActor1 : pPhysicsConstraintComp->ConstraintActor2);
	if(!IsValid(pGrabbedActor))
	{
		return;
	}

	

	const FGameplayTag UpdateResult = pGrabbedActor->TryUpdateInteraction(GetScriptInterface());
	if(UpdateResult == FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.Stop.Drop"))))
	{
		///pHandleComp->
		//TODO: WTF
		//CancelGrabbingAbility();
	}
}





