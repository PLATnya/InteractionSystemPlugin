// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UObject/Interface.h"
#include "ISCharacterInteraction.generated.h"

class AISInteractable;
class UPhysicsHandleComponent;
class UPhysicsConstraintComponent;
// This class does not need to be modified.
UINTERFACE()
class UISCharacterInteraction : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INTERACTIONSYSTEM_API IISCharacterInteraction
{
	GENERATED_BODY()
	
	TArray<AISInteractable*> ActiveItems;
	//TArray<AISInteractable*> ActiveItems;
	
public:
	void BeginInteraction();
	
	void UpdateInteraction(float _DeltaTime);
	
	//TODO: refresh

	virtual FVector GetGrabbingLocation() const = 0;
	
	//TODO: refresh

	virtual FRotator GetGrabbingRotation() const = 0;
	
	
	virtual void AddActiveItem(class AISInteractable* pItem) = 0;
	//TODO: refresh
	
	virtual void RemoveActiveItem(class AISInteractable* pItem) = 0;
	
	/*//UFUNCTION(BlueprintCallable, BlueprintPure)
	const TArray<AISInteractable*>& GetActiveItems() const { return ActiveItems; }
	
	//UFUNCTION(BlueprintCallable, BlueprintPure)
	const TArray<AISInteractable*> GetActiveItemsCopy() const { return ActiveItems; }*/

	
	virtual UPhysicsConstraintComponent* GetPhysicsConstraintComponent() const = 0;
	virtual UPhysicsHandleComponent* GetPhysicsHandleComponent() const = 0;
	//virtual UMInventoryComponent* GetInventoryComponent() const = 0;
	
	virtual float GetMaxGrabbingMass() const = 0;

	UFUNCTION()
	virtual void OnPhysicsConstraintBroken(int32 _ConstraintIndex);

	const TScriptInterface<IISCharacterInteraction>& GetScriptInterface();
private:

	TScriptInterface<IISCharacterInteraction> ScriptInterface;
	void UpdateHandle(float _DeltaTime);
	void UpdateConstraints(float _DeltaTime);
};
