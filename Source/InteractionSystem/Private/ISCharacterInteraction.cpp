// Fill out your copyright notice in the Description page of Project Settings.


#include "ISCharacterInteraction.h"

#include "ISInteractable.h"
#include "GameFramework/Character.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"


void IISCharacterInteraction::BeginInteraction()
{
	//
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
		pHandleComp->ReleaseComponent();
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
		pPhysicsConstraintComp->BreakConstraint();
	}
}





