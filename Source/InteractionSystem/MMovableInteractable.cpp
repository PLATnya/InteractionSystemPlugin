//


#include "MMovableInteractable.h"

#include "ISCharacterInteractionInterface.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"


FGameplayTag AMMovableInteractable::TryUpdateInteraction(IISCharacterInteractionInterface* pInstigatorCharacter)
{
	Super::TryUpdateInteraction(pInstigatorCharacter);

	const FGameplayTag ErrorTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Interaction.Stop.Error")));
	
	UPhysicsHandleComponent* pHandleComp = pInstigatorCharacter->GetPhysicsHandleComponent();
	if(!IsValid(pHandleComp))
	{
		return ErrorTag;
	}
	
	UPhysicsConstraintComponent* pPhysicsConstraintComp = pInstigatorCharacter->GetPhysicsConstraintComponent();
	if(!IsValid(pPhysicsConstraintComp))
	{
		return ErrorTag;
	}

	UPrimitiveComponent* pGrabbedComponent = IsValid(pHandleComp->GetGrabbedComponent()) ? pHandleComp->GetGrabbedComponent() : pPhysicsConstraintComp->OverrideComponent2.Get() ;
	if(!IsValid(pGrabbedComponent))
	{
		return ErrorTag;
	}

	if(!OwnsComponent(pGrabbedComponent))
	{
		return ErrorTag;
	}
	
	const FVector GrabbedComponentLocation = pGrabbedComponent->GetComponentLocation();
	
	const FVector GrabbingLocation = pInstigatorCharacter->GetGrabbingLocation();
	const FRotator GrabbingRotation = pInstigatorCharacter->GetGrabbingRotation();
	
	const FRotator LookAtRotation =  UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),GrabbingLocation);

	if(!IsHeavyFor(pInstigatorCharacter))
	{
		pHandleComp->SetTargetLocationAndRotation(GrabbingLocation, GrabbingRotation);
		return FGameplayTag::EmptyTag;
	}
	
	FVector OutLinearForce;
	FVector OutAngularForce;
	pPhysicsConstraintComp->ConstraintInstance.GetConstraintForce(OutLinearForce, OutAngularForce);

	float PushLinearFactor = 1.0f;
	if(BuffConstLinearForce != 0)
	{
		PushLinearFactor = OutLinearForce.Size() / BuffConstLinearForce;
	}

	float PushAngularFactor = 1.0f;
	if(BuffConstAngularForce != 0)
	{
		PushAngularFactor = OutAngularForce.Size() / BuffConstAngularForce;
	}
	
#if WITH_EDITOR	 
	UE_LOG(LogTemp, Warning, TEXT("LinearForce: %f %f %f  Length: %f"), OutLinearForce.X, OutLinearForce.Y, OutLinearForce.Z, OutLinearForce.Size());
	UE_LOG(LogTemp, Warning, TEXT("AngularForce: %f %f %f  Length: %f"), OutAngularForce.X, OutAngularForce.Y, OutAngularForce.Z, OutAngularForce.Size());
	//UE_LOG(LogTemp, Warning, TEXT("Avarage mass: %f"), pPhysicsConstraintComp->ConstraintInstance.AverageMass);
	UE_LOG(LogTemp, Warning, TEXT("Delta: %f,  factor: %f"), OutLinearForce.Size() - BuffConstLinearForce, PushLinearFactor);
#endif
	
	if(PushLinearFactor <= GetGrabbingLimitFactor())
	{
		BuffConstLinearForce = OutLinearForce.Size();
		//TODO: some delta Force mahinations
		return FGameplayTag::EmptyTag;
	}
	
	BuffConstLinearForce = 0.0f;
	BuffConstAngularForce = 0.0f;
	
#if WITH_EDITOR
	GetWorld()->bDebugPauseExecution = true;
#endif

	return FGameplayTag::RequestGameplayTag(FName(TEXT("Interactable.Stop.Drop")));
}

FVector AMMovableInteractable::GetGrabbedLocation() const
{
	const UMeshComponent* pMeshComp = GetMeshComponent();
	if (!ensure(IsValid(pMeshComp)))
	{
		return GetActorLocation();
	}
	
	return pMeshComp->GetSocketLocation(GrabbedSocket);
}

UPrimitiveComponent* AMMovableInteractable::GetMainPrimitiveComponent() const
{
	UMeshComponent* pMeshComp = GetMeshComponent();
	UPrimitiveComponent* pPrimitiveComp = Cast<UPrimitiveComponent>(GetRootComponent());
	if(!IsValid(pPrimitiveComp))
	{
		pPrimitiveComp = pMeshComp;
	}
	
	return pPrimitiveComp;
}


TArray<FName> AMMovableInteractable::GetSortedByDistancePrioritySockets(const FVector& Location) const
{

	const int32 SocketsCount = GrabbingSockets.Num();
	if(!ensure(SocketsCount>0)){ return TArray<FName>(); }

	const UMeshComponent* pMeshComp = GetMeshComponent();
	if(!ensure(IsValid(pMeshComp))){ return TArray<FName>(); }

	if(SocketsCount == 1){ return GrabbingSockets; }

	TArray<FName> SortedSockets = GrabbingSockets;
	for(int i = 1; i < SocketsCount; ++i)
	{
		for(int r = 0; r < SocketsCount-i; r++)
		{
			const float LeftElementDistance = FVector::Distance(Location, pMeshComp->GetSocketLocation(SortedSockets[r]));
			const float RightElementDistance = FVector::Distance(Location, pMeshComp->GetSocketLocation(SortedSockets[r+1]));
			if(LeftElementDistance >= RightElementDistance)
			{
				// Обмен местами
				FName temp = SortedSockets[r];
				SortedSockets[r] = SortedSockets[r+1];
				SortedSockets[r+1] = temp;
			}
		}
	}

	return SortedSockets;
}


bool AMMovableInteractable::IsHeavyFor(IISCharacterInteractionInterface* _pCharacter) const
{
	return GetMass() > _pCharacter->GetMaxGrabbingMass();
}

bool AMMovableInteractable::TryDrop_Implementation(IISCharacterInteractionInterface* pCharacter)
{	
	if(!pCharacter){ return false; }
	
	if(!Super::TryDrop_Implementation(pCharacter))
	{
		return false;
	}
	
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	pCharacter->RemoveActiveItem(this);
	
	UPhysicsHandleComponent* pPhysicHandleComp = pCharacter->GetPhysicsHandleComponent();
	if(!ensure(IsValid(pPhysicHandleComp))){ return false; }

	UPhysicsConstraintComponent* pPhysicsConst = pCharacter->GetPhysicsConstraintComponent();
	if(!ensure(IsValid(pPhysicsConst))){ return false; }
	
	if(OwnsComponent(pPhysicHandleComp->GetGrabbedComponent()))
	{
		pPhysicHandleComp->ReleaseComponent();
	}
	else if(OwnsComponent(pPhysicsConst->OverrideComponent1.Get()) || OwnsComponent(pPhysicsConst->OverrideComponent2.Get()))
	{
		if(! pPhysicsConst->IsBroken())
		{
			pPhysicsConst->BreakConstraint();
		}
	}

	UPrimitiveComponent* pPrimitiveComp = GetMainPrimitiveComponent();
	if (!ensure(IsValid(pPrimitiveComp))){ return false; }

	
	AActor* pCharacterActor = Cast<AActor>(pCharacter);
	if(!ensure(IsValid(pCharacterActor))){ return false; }
	
	pPrimitiveComp->IgnoreActorWhenMoving(pCharacterActor, false);
	return true;
}


void AMMovableInteractable::GetInHands_Implementation(IISCharacterInteractionInterface* pCharacter)
{
	if(!pCharacter){ return; }
	
	const UWorld* pWorld = GetWorld();
	if(!ensure(IsValid(pWorld))){ return; }
	
	ACharacter* pCharacterActor = Cast<ACharacter>(pCharacter);
	if(!ensure(IsValid(pCharacterActor))){ return; }
	
	if(!ensure(IsValid(pCharacterActor))){ return; }
	
	auto SimpleGrabbingAction = [=]()
	{
		AttachToActor(pCharacterActor, FAttachmentTransformRules::KeepRelativeTransform);
		Super::GetInHands_Implementation(pCharacter);
	};
	
	const USkeletalMeshComponent* pCharacterMeshComp = pCharacterActor->GetMesh();
	if (!ensure(IsValid(pCharacterMeshComp)))
	{
		SimpleGrabbingAction();
		return;
	}

	UPhysicsHandleComponent* pPhysicsHandleComp = pCharacter->GetPhysicsHandleComponent();
	if (!ensure(IsValid(pPhysicsHandleComp)))
	{
		SimpleGrabbingAction();
		return;
	}


	UPhysicsConstraintComponent* pPhysicsConst = pCharacter->GetPhysicsConstraintComponent();
	if(!ensure(IsValid(pPhysicsConst)))
	{
		SimpleGrabbingAction();
		return;
	}
	
	const UMeshComponent* pMeshComp = GetMeshComponent();
	if (!ensure(IsValid(pMeshComp)))
	{
		SimpleGrabbingAction();
		return;
	}
	
	UPrimitiveComponent* pPrimitiveComp = GetMainPrimitiveComponent();
	if (!ensure(IsValid(pPrimitiveComp)))
	{
		SimpleGrabbingAction();
		return;
	}
	
	pPrimitiveComp->IgnoreActorWhenMoving(pCharacterActor, true);
	
	const FVector CharacterLocation = pCharacterActor->GetActorLocation();
	TArray<FName> SocketsArray = GetSortedByDistancePrioritySockets(CharacterLocation);
	
	if(!IsHeavyFor(pCharacter))
	{
		const FVector GrabbingLocation = SocketsArray.Num() > 0 ? pMeshComp->GetSocketLocation(SocketsArray[0]) : pPrimitiveComp->GetComponentLocation();
		const FRotator GrabbingRotation = SocketsArray.Num() > 0 ? pMeshComp->GetSocketRotation(SocketsArray[0]) : pPrimitiveComp->GetComponentRotation();
		
		pPhysicsHandleComp->GrabComponentAtLocationWithRotation(pPrimitiveComp, NAME_None, GrabbingLocation, GrabbingRotation);
		pCharacter->AddActiveItem(this);
	}
	else
	{
		UCapsuleComponent* pCharacterCapsule = pCharacterActor->GetCapsuleComponent();
		if(!ensure(IsValid(pCharacterCapsule))){ return; }

		const float CheckCapsuleHalfHeight = pCharacterActor->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		const float CheckCapsuleRadius = pCharacterCapsule->GetScaledCapsuleRadius();

		const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes{UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_MAX)};
		const TArray<AActor*> ActorsToIgnore{pCharacterActor};
		
		const auto* AvailableSocketName = SocketsArray.FindByPredicate([&](const FName& SocketName)
		{
			const FVector GrabbingSocketLocation = pMeshComp->GetSocketLocation(SocketName);
			TArray<AActor*> OverlappedActors;
			return !UKismetSystemLibrary::CapsuleOverlapActors(pWorld, GrabbingSocketLocation, CheckCapsuleRadius, CheckCapsuleHalfHeight,ObjectTypes,
				nullptr,ActorsToIgnore,OverlappedActors);
		});

		if(!AvailableSocketName)
		{
			return;
		}
		
		FRotator TargetCharacterLocation = UKismetMathLibrary::FindLookAtRotation(CharacterLocation, pPrimitiveComp->GetComponentLocation());
		TargetCharacterLocation.Pitch = 0;
		TargetCharacterLocation.Roll = 0;
		pCharacterActor->SetActorRotation(TargetCharacterLocation);
		pCharacterActor->SetActorLocation(pMeshComp->GetSocketLocation(*AvailableSocketName));
		
		pWorld->GetTimerManager().SetTimerForNextTick([=]()
		{
			GrabbedSocket = *AvailableSocketName;
			pPhysicsConst->ConstraintActor1 = pCharacterActor;
			pPhysicsConst->ConstraintActor2 = this;
			pPhysicsConst->SetConstrainedComponents(pCharacterCapsule, NAME_None, pPrimitiveComp, NAME_None);
	
		});
	}
	
	Super::GetInHands_Implementation(pCharacter);
}
