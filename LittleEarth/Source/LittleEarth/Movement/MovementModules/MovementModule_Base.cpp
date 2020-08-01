#include "MovementModule_Base.h"
#include "Utils/LogManager.h"

UMovementModule_Base::UMovementModule_Base(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	MaxVelocity = 600.f;
	MaxAcceleration = 2048.f;
	Friction = 3.f;
	BrakingDeceleration = 0.0f;
}

void UMovementModule_Base::CalcVelocity(FMovementContext_Eval& context, const FMovementContext_Character& characterContext, FFindFloorResult& CurrentFloor, UMovementComponent* movementComponent) const {

	auto friction = FMath::Max(0.f, Friction);

	auto inputAcceleration = CalcInputAcceleration(context, characterContext, CurrentFloor, movementComponent);
	auto frictionDeceleration = (-friction) * context.Velocity;

	context.Velocity += context.DeltaTime * inputAcceleration;	// Apply Input
	context.Velocity += context.DeltaTime * frictionDeceleration;	// Apply friction

	// Apply braking if no input (character can brake only X and Y parts)
	if (inputAcceleration.IsZero()) {
		auto horVelocity = FVector(context.Velocity.X, context.Velocity.Y, 0);
		auto braking = FMath::Max(0.f, BrakingDeceleration); // Apply braking
		auto appliedBraking = FMath::Min(braking, horVelocity.Size());
		if (appliedBraking > 0)
			context.Velocity += (-appliedBraking) * horVelocity.GetSafeNormal();
	}

	context.Velocity += context.DeltaTime * CalcExternalAcceleration(context, characterContext, CurrentFloor, movementComponent); // Apply Outer Forces (Gravity, etc)

	// Clamp to zero if nearly zero, or if below min threshold and braking.
	if (context.Velocity.SizeSquared() <= KINDA_SMALL_NUMBER) {
		context.Velocity = FVector::ZeroVector;
	}
}

FVector UMovementModule_Base::CalcInputAcceleration(const FMovementContext_Eval& context, const FMovementContext_Character& characterContext, FFindFloorResult& CurrentFloor, UMovementComponent* movementComponent) const {
	auto constraintAcceleration = FVector(context.InputVector.X, context.InputVector.Y, 0.f);

	auto maxVelocity = FMath::Max(0.f, MaxVelocity);

	auto maxAppliedAcceleration = FMath::Max(0.f, MaxAcceleration);

	// acceleration can not exceed velocity limit
	if ((context.Velocity | context.InputVector) > 0) {
		maxAppliedAcceleration = FMath::Min(maxAppliedAcceleration, FMath::Max(0.0f, maxVelocity - context.Velocity.Size()) / context.DeltaTime);
	}
	return constraintAcceleration.GetClampedToMaxSize(1.0f) * maxAppliedAcceleration;
}