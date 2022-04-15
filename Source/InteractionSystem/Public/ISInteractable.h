// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTags/Classes/GameplayTagAssetInterface.h"
#include "GameFramework/Actor.h"
#include "ISInteractable.generated.h"

struct FGameplayTag;
struct FGameplayTagContainer;
class IISCharacterInteraction;
UCLASS()
class INTERACTIONSYSTEM_API AISInteractable : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:

	//tag asset interface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	
	// Sets default values for this actor's properties
	AISInteractable();

	//UFUNCTION(BlueprintNativeEvent)
	void GetInHands( IISCharacterInteraction*pCharacter );

	//UFUNCTION(BlueprintNativeEvent)
	bool TryDrop( IISCharacterInteraction* pCharacter );

	//UFUNCTION(BlueprintNativeEvent)
	void Interact(AActor* pInstigator);
	
	//UFUNCTION(BlueprintNativeEvent)
	bool TryStopInteract(AActor* pInstigator);
	
	
	UFUNCTION()
	void SetActive(bool bIsActive);

	UFUNCTION(BlueprintNativeEvent)
	void Break(AActor* pInstigator);
	
	UFUNCTION()
	bool IsActiveInGame() const { return bIsActiveInGame; }

	
	UFUNCTION()
	bool IsGrabbed() const;

	UFUNCTION()
	bool IsBroken() const;

	UFUNCTION()
	bool IsInInteraction() const;

	//TODO: make some static predetermined place with tags and get it from there instead requesting of it every time
	
	UFUNCTION()
	void SetIsTargetable(bool _bIsTargetable);
	//virtual bool IsTargetable_Implementation() const override;

	//UFUNCTION(BlueprintNativeEvent)
	UMeshComponent* GetMeshComponent() const;

	UFUNCTION()
	virtual FGameplayTag TryUpdateInteraction(const TScriptInterface<IISCharacterInteraction>& pInstigatorCharacter);

protected:
	void AddInteractableTag(const FGameplayTag& _Tag);
	void RemoveInteractableTag(const FGameplayTag& _Tag);


private:
	FGameplayTagContainer InteractableTags;
	
	bool bIsActiveInGame = true;
	bool bIsTargetable = true;
};
