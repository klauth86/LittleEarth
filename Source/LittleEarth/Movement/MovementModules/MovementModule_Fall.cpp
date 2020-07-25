// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementModule_Fall.h"
#include "GameFramework/MovementComponent.h"

UMovementModule_Fall::UMovementModule_Fall(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	MaxVelocity = 600.f;
	MaxAcceleration = 128.f;
	Friction = 1.f;
	BrakingDeceleration = 0.0f;
}

void UMovementModule_Fall::ApplyMovement(FMovementContext_Eval& context, const FMovementContext_Character& characterContext, FFindFloorResult& CurrentFloor, UMovementComponent* movementComponent) const {
	FHitResult Hit(1.f);
	auto updatedComponent = movementComponent->UpdatedComponent;

	auto Delta = context.GetDelta();
	movementComponent->SafeMoveUpdatedComponent(Delta, updatedComponent->GetComponentQuat(), true, Hit); // Simple moving

	if (Hit.IsValidBlockingHit()) {
		OnValidBlockingHit(context, Hit);	// Correct for first blocking hit and try again
		Delta = context.GetDelta();
		movementComponent->SafeMoveUpdatedComponent(Delta, updatedComponent->GetComponentQuat(), true, Hit); // Simple moving
	}
}