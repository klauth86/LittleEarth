// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementModule_Base.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MovementModule_Walk.generated.h"

/**
 * 
 */
UCLASS()
class UMovementModule_Walk : public UMovementModule_Base
{
	GENERATED_BODY()
	
public:

	virtual void ApplyMovement(FMovementContext_Eval& context, const FMovementContext_Character& characterContext, FFindFloorResult& CurrentFloor, UMovementComponent* movementComponent) const override;

	virtual void Adjust(const FMovementContext_Eval& context, const FMovementContext_Character& characterContext, FFindFloorResult& CurrentFloor, UMovementComponent* movementComponent) const override;

protected:

	virtual void MoveAlongFloor(UCharacterMovementComponent::FStepDownResult* OutStepDownResult, FMovementContext_Eval& context, const FMovementContext_Character& characterContext, FFindFloorResult& CurrentFloor, UMovementComponent* movementComponent) const;

	bool StepUp(UCharacterMovementComponent::FStepDownResult* OutStepDownResult, FMovementContext_Eval& context, const FMovementContext_Character& characterContext,
		FFindFloorResult& CurrentFloor, const FHitResult &InHit, float remainingMovementPercent, UMovementComponent* movementComponent,
		UCapsuleComponent* capsuleComponent = NULL, APawn* pawnOwner = NULL) const;


	void PhysicsRotation(const FMovementContext_Eval& context, const FMovementContext_Character& characterContext, UMovementComponent* movementComponent) const;

	FRotator GetDeltaRotation(float DeltaTime, FRotator RotationRate) const;

	FRotator ComputeOrientToMovementRotation(const FRotator& CurrentRotation, float DeltaTime, FRotator& DeltaRotation, FVector Acceleration) const;
};
