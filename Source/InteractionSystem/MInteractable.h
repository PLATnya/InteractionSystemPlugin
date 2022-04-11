// 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
//#include "TargetSystem/Public/TargetSystemTargetableInterface.h"
#include "MInteractable.generated.h"

class IISCharacterInteractionInterface;

UCLASS()
class ALSV2_API AMInteractable : public AActor, /*public ITargetSystemTargetableInterface,*/ public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:

	//tag asset interface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	
	// Sets default values for this actor's properties
	AMInteractable();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetInHands( IISCharacterInteractionInterface* pCharacter );

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool TryDrop( IISCharacterInteractionInterface* pCharacter );

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Interact(AActor* pInstigator);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool TryStopInteract(AActor* pInstigator);
	
	
	UFUNCTION(BlueprintCallable)
	void SetActive(bool bIsActive);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Break(AActor* pInstigator);
	
	UFUNCTION(BlueprintCallable)
	bool IsActiveInGame() const { return bIsActiveInGame; }

	
	UFUNCTION(BlueprintCallable)
	bool IsGrabbed() const;

	UFUNCTION(BlueprintCallable)
	bool IsBroken() const;

	UFUNCTION(BlueprintCallable)
	bool IsInInteraction() const;

	//TODO: make some static predetermined place with tags and get it from there instead requesting of it every time
	
	UFUNCTION(BlueprintCallable)
	void SetIsTargetable(bool _bIsTargetable);
	//virtual bool IsTargetable_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UMeshComponent* GetMeshComponent() const;

	UFUNCTION()
	virtual FGameplayTag TryUpdateInteraction(IISCharacterInteractionInterface* pInstigatorCharacter);

protected:
	void AddInteractableTag(const FGameplayTag& _Tag);
	void RemoveInteractableTag(const FGameplayTag& _Tag);


private:
	FGameplayTagContainer InteractableTags;
	
	bool bIsActiveInGame = true;
	bool bIsTargetable = true;
};
