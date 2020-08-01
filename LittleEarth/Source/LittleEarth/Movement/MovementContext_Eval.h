#pragma once

#include "Vector.h"
#include "Dicts/RotationUpdateMode.h"

struct FMovementContext_Eval {

	FVector InputVector;

	FVector Velocity;

	float DeltaTime;

	FVector CorrectionVelocity; // Correction if needed

	bool HasOverrideVelocity2D;

	FVector OverrideVelocity2D;

	// Additional Flags

	ERotationUpdateMode RotationUpdateMode = ERotationUpdateMode::DISABLED;

	FMovementContext_Eval() {
		Clear();
	}

	void RemoveProjectionFromMovement(FVector projection) {
		Velocity -= (Velocity | projection)* projection;
		CorrectionVelocity -= (CorrectionVelocity | projection)* projection;
		OverrideVelocity2D -= (OverrideVelocity2D | projection)* projection;
	}

	void Clear() {
		InputVector = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		DeltaTime = 0.0f;
		CorrectionVelocity = FVector::ZeroVector;

		HasOverrideVelocity2D = false;
		OverrideVelocity2D = FVector::ZeroVector;

		RotationUpdateMode = ERotationUpdateMode::DISABLED;
	}

	FVector GetDelta() const {
		return HasOverrideVelocity2D ? GetOverrideDelta() : GetPrimaryDelta();
	}

	FVector GetPrimaryDelta() const {
		return (Velocity + CorrectionVelocity) * DeltaTime;
	}

	FVector GetOverrideDelta() const {
		return (OverrideVelocity2D + FVector(0, 0, Velocity.Z) + CorrectionVelocity) * DeltaTime;
	}

	bool HasMovement() const {
		return !GetDelta().IsNearlyZero();
	}

	bool HasPrimaryMovement() const {
		return !GetPrimaryDelta().IsNearlyZero();
	}

	bool HasOverrideMovement() const {
		return !GetOverrideDelta().IsNearlyZero();
	}

	bool HasInput() const {
		return !InputVector.IsNearlyZero();
	}
};