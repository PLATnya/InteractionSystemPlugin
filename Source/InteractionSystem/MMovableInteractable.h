#pragma once

#include "CoreMinimal.h"
#include "MInteractable.h"
#include "MMovableInteractable.generated.h"

class IISCharacterInteractionInterface;
UCLASS()
class ALSV2_API AMMovableInteractable : public AMInteractable
{
	GENERATED_BODY()
public:
	virtual FGameplayTag TryUpdateInteraction(IISCharacterInteractionInterface* pInstigatorCharacter) override;
	
	FVector GetGrabbedLocation() const;
	
	UPrimitiveComponent* GetMainPrimitiveComponent() const;
	
	UFUNCTION(BlueprintCallable)
	bool IsHeavyFor(class IISCharacterInteractionInterface* _pCharacter) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual float GetMass() const{ return Mass; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual int32 GetCapacity() const{ return Capacity; }

	virtual bool TryDrop_Implementation(IISCharacterInteractionInterface* pCharacter) override;
	virtual void GetInHands_Implementation(IISCharacterInteractionInterface* pCharacter) override;

	UFUNCTION(BlueprintCallable, Category="Grabbing")
	float GetGrabbingLimitFactor() const { return GrabbingLimitFactor; }

	UFUNCTION(BlueprintCallable)
	TArray<FName> GetSortedByDistancePrioritySockets(const FVector& Location) const;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Grabbing")
	FName GrabbedSocket = NAME_None;

	UPROPERTY(EditDefaultsOnly, Category="Grabbing")
	TArray<FName> GrabbingSockets;
	
	//if ration of previous and current force applied by joint to this object greater then this limit, joint will broke
	UPROPERTY(EditAnywhere, Category="Grabbing")
	float GrabbingLimitFactor = 1.0f;
private:
	UPROPERTY(EditAnywhere)
	float Mass = 0.0f;
	UPROPERTY(EditAnywhere)
	int32 Capacity = 0.0f;
	
	float BuffConstLinearForce = 0.0f;
	float BuffConstAngularForce = 0.0f;
};