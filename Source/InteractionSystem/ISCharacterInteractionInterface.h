// 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ISCharacterInteractionInterface.generated.h"

class AMInteractable;
class UPhysicsConstraintComponent;
class UPhysicsHandleComponent;
// This class does not need to be modified.
UINTERFACE()
class UISCharacterInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INTERACTIONSYSTEM_API IISCharacterInteractionInterface
{
	GENERATED_BODY()

	TArray<AMInteractable*> ActiveItems;
	
public:
	void BeginInteraction();
	
	void UpdateInteraction(float _DeltaTime);
	
	//TODO: refresh
	UFUNCTION(BlueprintNativeEvent)
	FVector GetGrabbingLocation() const;
	
	//TODO: refresh
	UFUNCTION(BlueprintNativeEvent)
	FRotator GetGrabbingRotation() const;
	
	UFUNCTION(BlueprintCallable)
	void AddActiveItem(class AMInteractable* pItem);
	//TODO: refresh
	UFUNCTION(BlueprintCallable)
	void RemoveActiveItem(class AMInteractable* pItem);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const TArray<AMInteractable*>& GetActiveItems() const { return ActiveItems; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	const TArray<AMInteractable*> GetActiveItemsCopy() const { return ActiveItems; }

	
	virtual UPhysicsConstraintComponent* GetPhysicsConstraintComponent() const = 0;
	virtual UPhysicsHandleComponent* GetPhysicsHandleComponent() const = 0;

	virtual void OnPhysicsConstraintBroken(int32 _ConstraintIndex);
private:
	void UpdateHandle(float _DeltaTime);
	void UpdateConstraints(float _DeltaTime);
};
