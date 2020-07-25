// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementModule_Walk.h"
#include "Engine/EngineTypes.h"
#include "Components/CapsuleCOmponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Utils/FloorHelper.h"
#include "ProjectSettingsFacade.h"
#include "Utils/LogManager.h"

void UMovementModule_Walk::ApplyMovement(FMovementContext_Eval& context, const FMovementContext_Character& characterContext, FFindFloorResult& CurrentFloor, UMovementComponent* movementComponent) const {
	
	// TODO
	if (context.HasMovement()) {
		UCharacterMovementComponent::FStepDownResult StepDownResult;
		MoveAlongFloor(&StepDownResult, context, characterContext, CurrentFloor, movementComponent);
	}

	if (context.HasOverrideMovement() || context.HasPrimaryMovement()) {
		if (context.RotationUpdateMode != ERotationUpdateMode::DISABLED)
			PhysicsRotation(context, characterContext, movementComponent);
	}
}

void UMovementModule_Walk::Adjust(const FMovementContext_Eval& context, const FMovementContext_Character& characterContext, FFindFloorResult& CurrentFloor, UMovementComponent* movementComponent) const {
	
	if (!CurrentFloor.IsWalkableFloor()) {	// If we have a floor check that hasn't hit anything, don't adjust height.
		return;
	}

	float OldFloorDist = CurrentFloor.FloorDist;

	if (CurrentFloor.bLineTrace) {
		if (OldFloorDist < UCharacterMovementComponent::MIN_FLOOR_DIST && CurrentFloor.LineDist >= UCharacterMovementComponent::MIN_FLOOR_DIST) {
			return;
		}
		else {
			OldFloorDist = CurrentFloor.LineDist;
		}
	}

	auto updatedComponent = movementComponent->UpdatedComponent;

	// Move up or down to maintain floor height.
	if (OldFloorDist < UCharacterMovementComponent::MIN_FLOOR_DIST || OldFloorDist > UCharacterMovementComponent::MAX_FLOOR_DIST) {
		FHitResult AdjustHit(1.f);
		const float InitialZ = updatedComponent->GetComponentLocation().Z;
		const float AvgFloorDist = (UCharacterMovementComponent::MIN_FLOOR_DIST + UCharacterMovementComponent::MAX_FLOOR_DIST) * 0.5f;
		const float MoveDist = AvgFloorDist - OldFloorDist;
		movementComponent->SafeMoveUpdatedComponent(FVector(0.f, 0.f, MoveDist), updatedComponent->GetComponentQuat(), true, AdjustHit);

		if (!AdjustHit.IsValidBlockingHit()) {
			CurrentFloor.FloorDist += MoveDist;
		}
		else if (MoveDist > 0.f) {
			const float CurrentZ = updatedComponent->GetComponentLocation().Z;
			CurrentFloor.FloorDist += CurrentZ - InitialZ;
		}
		else {
			checkSlow(MoveDist < 0.f);
			const float CurrentZ = updatedComponent->GetComponentLocation().Z;
			CurrentFloor.FloorDist = CurrentZ - AdjustHit.Location.Z;

			if (IsWalkable(AdjustHit, characterContext)) {
				CurrentFloor.SetFromSweep(AdjustHit, CurrentFloor.FloorDist, true);
			}
		}
	}
}

// Movement

void UMovementModule_Walk::MoveAlongFloor(UCharacterMovementComponent::FStepDownResult* OutStepDownResult, FMovementContext_Eval& context, 
	const FMovementContext_Character& characterContext, FFindFloorResult& CurrentFloor, UMovementComponent* movementComponent) const {

	if (!CurrentFloor.IsWalkableFloor()) {
		return;
	}

	auto updatedComponent = movementComponent->UpdatedComponent;
	auto capsuleComponent = Cast<UCapsuleComponent>(updatedComponent);

	auto Delta = context.GetDelta();

	// Move along the current floor
	FHitResult Hit(1.f);
	FVector RampVector = ComputeGroundMovementDelta(Delta, CurrentFloor.HitResult, CurrentFloor.bLineTrace, characterContext);

	movementComponent->SafeMoveUpdatedComponent(RampVector, updatedComponent->GetComponentQuat(), true, Hit);
	float LastMoveTimeSlice = context.DeltaTime;

	if (Hit.bStartPenetrating) {

		// Allow this hit to be used as an impact we can deflect off, otherwise we do nothing the rest of the update and appear to hitch.
		movementComponent->HandleImpact(Hit);
		movementComponent->SlideAlongSurface(Delta, 1.f, Hit.Normal, Hit, true);
	}
	else if (Hit.IsValidBlockingHit()) {

		// We impacted something (most likely another ramp, but possibly a barrier).
		float appliedDeltaTime = Hit.Time;
		const float remainingDeltaTime = 1.f - appliedDeltaTime;

		if ((Hit.Time > 0.f) && (Hit.Normal.Z > KINDA_SMALL_NUMBER) && IsWalkable(Hit, characterContext)) {
			
			// Another walkable ramp.
			RampVector = ComputeGroundMovementDelta(Delta * remainingDeltaTime, Hit, false, characterContext);

			LastMoveTimeSlice = remainingDeltaTime * LastMoveTimeSlice;
			movementComponent->SafeMoveUpdatedComponent(RampVector, updatedComponent->GetComponentQuat(), true, Hit);

			const float SecondHitPercent = Hit.Time * remainingDeltaTime;
			appliedDeltaTime = FMath::Clamp(appliedDeltaTime + SecondHitPercent, 0.f, 1.f);
		}

		if (Hit.IsValidBlockingHit()) {
			
			auto pawnMovementComponent = Cast<UPawnMovementComponent>(movementComponent);
			auto pawnOwner = pawnMovementComponent ? pawnMovementComponent->GetPawnOwner() : NULL;

			if (CanStepUp(Hit, pawnOwner) &&
				StepUp(OutStepDownResult, context, characterContext, CurrentFloor, Hit, 1.f - appliedDeltaTime,
					movementComponent, capsuleComponent, pawnOwner)) 
				return;

			// Default calls for blocked movement
			movementComponent->HandleImpact(Hit, LastMoveTimeSlice, RampVector);
			movementComponent->SlideAlongSurface(Delta, 1.f - appliedDeltaTime, Hit.Normal, Hit, true);

			context.DeltaTime = 0; // Movement is blocked, however player can have nonzero velocity
		}
	}
}

bool UMovementModule_Walk::StepUp(UCharacterMovementComponent::FStepDownResult* OutStepDownResult, FMovementContext_Eval& context, 
	const FMovementContext_Character& characterContext, FFindFloorResult& CurrentFloor, const FHitResult &InHit, float remainingMovementPercent,
	UMovementComponent* movementComponent, UCapsuleComponent* capsuleComponent, APawn* pawnOwner) const {

	const auto appliedDeltaTime = context.DeltaTime * (1 - remainingMovementPercent); // movement that was executed before collision
	const auto remainingDeltaTime = context.DeltaTime * remainingMovementPercent; // movement that was executed before collision

	auto Delta = context.GetDelta() * remainingMovementPercent;

	if (!capsuleComponent)
		capsuleComponent = Cast<UCapsuleComponent>(movementComponent->UpdatedComponent);

	if (!pawnOwner) {
		auto pawnMovementComponent = Cast<UPawnMovementComponent>(movementComponent);
		pawnOwner = pawnMovementComponent ? pawnMovementComponent->GetPawnOwner() : NULL;
	}

	if (!CanStepUp(InHit, pawnOwner) || characterContext.MaxStepHeight <= 0.f) {
		return false;
	}

	const FVector OldLocation = capsuleComponent->GetComponentLocation();
	
	float PawnRadius, PawnHalfHeight;
	capsuleComponent->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);

	const float InitialImpactZ = InHit.ImpactPoint.Z;	// Don't bother stepping up if top of capsule is hitting something.
	if (InitialImpactZ > OldLocation.Z + (PawnHalfHeight - PawnRadius)) {
		return false;
	}

	auto gravDir = UProjectSettingsFacade::GetGravityDir(movementComponent->GetOwner()->GetActorLocation());
	const auto multiplier = 1.25f;
	auto stepTest = characterContext.MaxStepHeight * multiplier;

	const float StepSideZ = -1.f * FVector::DotProduct(InHit.ImpactNormal, gravDir);
	
	float PawnInitialFloorBaseZ = OldLocation.Z - PawnHalfHeight;
	float PawnFloorPointZ = PawnInitialFloorBaseZ;

	if (CurrentFloor.IsWalkableFloor()) {
		// Since we float a variable amount off the floor, we need to enforce max step height off the actual point of impact with the floor.
		const float FloorDist = FMath::Max(0.f, CurrentFloor.GetDistanceToFloor());		
		
		PawnInitialFloorBaseZ -= FloorDist;

		const bool bHitVerticalFace = !IsWithinEdgeTolerance(InHit.Location, InHit.ImpactPoint, PawnRadius);
		if (!CurrentFloor.bLineTrace && !bHitVerticalFace) {
			PawnFloorPointZ = CurrentFloor.HitResult.ImpactPoint.Z;
		}
		else {
			// Base floor point is the base of the capsule moved down by how far we are hovering over the surface we are hitting.
			PawnFloorPointZ -= CurrentFloor.FloorDist;
		}
	}

	// Don't step up if the impact is below us, accounting for distance from floor.
	if (InitialImpactZ <= PawnInitialFloorBaseZ) {
		return false;
	}

	// Scope our movement updates, and do not apply them until all intermediate moves are completed.
	FScopedMovementUpdate ScopedStepUpMovement(capsuleComponent, EScopedUpdate::DeferredUpdates);

	// step up - treat as vertical wall
	FHitResult SweepUpHit(1.f);
	const FQuat PawnRotation = capsuleComponent->GetComponentQuat();
	movementComponent->MoveUpdatedComponent(-gravDir * stepTest, PawnRotation, true, &SweepUpHit);

	if (SweepUpHit.bStartPenetrating) {
		ScopedStepUpMovement.RevertMove();	// Undo movement
		return false;
	}

	// step fwd
	FHitResult Hit(1.f);
	movementComponent->MoveUpdatedComponent(Delta, PawnRotation, true, &Hit);

	if (Hit.bBlockingHit) {	// Check result of forward movement

		if (Hit.bStartPenetrating) {
			ScopedStepUpMovement.RevertMove();	// Undo movement
			return false;
		}

		// If we hit something above us and also something ahead of us, we should notify about the upward hit as well.
		// The forward hit will be handled later (in the bSteppedOver case below).
		// In the case of hitting something above but not forward, we are not blocked from moving so we don't need the notification.
		if (SweepUpHit.bBlockingHit && Hit.bBlockingHit) {
			movementComponent->HandleImpact(SweepUpHit);
		}

		movementComponent->HandleImpact(Hit);	// pawn ran into a wall

		// adjust and try again
		const float ForwardHitTime = Hit.Time;
		const float ForwardSlideAmount = movementComponent->SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);

		context.DeltaTime = appliedDeltaTime;

		// If both the forward hit and the deflection got us nowhere, there is no point in this step up.
		if (ForwardSlideAmount == 0.f) {
			ScopedStepUpMovement.RevertMove();
			return false;
		}
		else {
			context.DeltaTime += (1.f - Hit.Time) * remainingDeltaTime;
		}
	}

	// Step down
	movementComponent->MoveUpdatedComponent(gravDir * stepTest, capsuleComponent->GetComponentQuat(), true, &Hit);

	if (Hit.bStartPenetrating) {	// Undo movement
		ScopedStepUpMovement.RevertMove();
		return false;
	}

	UCharacterMovementComponent::FStepDownResult StepDownResult;
	if (Hit.IsValidBlockingHit()) {
		
		// See if this step sequence would have allowed us to travel higher than our max step height allows.
		const float DeltaZ = Hit.ImpactPoint.Z - PawnFloorPointZ;
		if (DeltaZ > characterContext.MaxStepHeight * multiplier) {
			ScopedStepUpMovement.RevertMove();
			return false;
		}

		// Reject unwalkable surface normals here.
		if (!IsWalkable(Hit, characterContext)) {
			// Reject if normal opposes movement direction
			const bool bNormalTowardsMe = (Delta | Hit.ImpactNormal) < 0.f;
			if (bNormalTowardsMe) {
				//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (unwalkable normal %s opposed to movement)"), *Hit.ImpactNormal.ToString());
				ScopedStepUpMovement.RevertMove();
				return false;

			}

			// Also reject if we would end up being higher than our starting location by stepping down.
			// It's fine to step down onto an unwalkable normal below us, we will just slide off. Rejecting those moves would prevent us from being able to walk off the edge.
			if (Hit.Location.Z > OldLocation.Z) {
				//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (unwalkable normal %s above old position)"), *Hit.ImpactNormal.ToString());
				ScopedStepUpMovement.RevertMove();
				return false;
			}
		}

		// Reject moves where the downward sweep hit something very close to the edge of the capsule. This maintains consistency with FindFloor as well.
		if (!IsWithinEdgeTolerance(Hit.Location, Hit.ImpactPoint, PawnRadius)) {
			//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (outside edge tolerance)"));
			ScopedStepUpMovement.RevertMove();
			return false;
		}

		// Don't step up onto invalid surfaces if traveling higher.
		if (DeltaZ > 0.f && !CanStepUp(Hit, pawnOwner)) {
			//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (up onto surface with !CanStepUp())"));
			ScopedStepUpMovement.RevertMove();
			return false;
		}

		// See if we can validate the floor as a result of this step down. In almost all cases this should succeed, and we can avoid computing the floor outside this method.
		if (OutStepDownResult != NULL) {
			UpdateFloor(StepDownResult.FloorResult, capsuleComponent, characterContext);

			// Reject unwalkable normals if we end up higher than our initial height.
			// It's fine to walk down onto an unwalkable surface, don't reject those moves.
			if (Hit.Location.Z > OldLocation.Z) {
				// We should reject the floor result if we are trying to step up an actual step where we are not able to perch (this is rare).
				// In those cases we should instead abort the step up and try to slide along the stair.
				
				//// TODO CHECK
				////if (!StepDownResult.FloorResult.bBlockingHit && StepSideZ < MAX_STEP_SIDE_Z) {
				////	ScopedStepUpMovement.RevertMove();
				////	return false;
				////}

			}
			StepDownResult.bComputedFloor = true;
		}
	}

	// Copy step down result.
	if (OutStepDownResult != NULL) {
		*OutStepDownResult = StepDownResult;
	}

	return true;
}


// Orient to movement/Control rotation

void UMovementModule_Walk::PhysicsRotation(const FMovementContext_Eval& context, const FMovementContext_Character& characterContext, UMovementComponent* movementComponent) const {

	auto updatedComponent = movementComponent->UpdatedComponent;
	auto pawnMovementComponent = Cast<UPawnMovementComponent>(movementComponent);
	auto pawnOwner = pawnMovementComponent->GetPawnOwner();

	if (!(characterContext.bOrientRotationToMovement || characterContext.bUseControllerDesiredRotation)) {
		return;
	}

	FRotator CurrentRotation = updatedComponent->GetComponentRotation(); // Normalized

	FRotator DeltaRot = GetDeltaRotation(context.DeltaTime, characterContext.RotationRate);

	FRotator DesiredRotation = CurrentRotation;
	if (characterContext.bOrientRotationToMovement) {
		if (context.RotationUpdateMode == ERotationUpdateMode::OVERRIDE) {
			DesiredRotation = ComputeOrientToMovementRotation(CurrentRotation, context.DeltaTime, DeltaRot, context.GetOverrideDelta());
		}
		else {
			DesiredRotation = ComputeOrientToMovementRotation(CurrentRotation, context.DeltaTime, DeltaRot, context.GetPrimaryDelta());
		}
	}
	else if (characterContext.bUseControllerDesiredRotation) {
		DesiredRotation = pawnOwner->Controller->GetDesiredRotation();
	}
	else {
		return;
	}

	////LogManager::LogWarning(TEXT("DES ROT %s, CORRECTION %s, VELOCITY %s, DELTA %s"), *DesiredRotation.ToCompactString(), *context.CorrectionVelocity.ToCompactString(), *context.Velocity.ToCompactString(),
	//	*context.GetDelta().ToCompactString());

	DesiredRotation.Pitch = 0.f;
	DesiredRotation.Yaw = FRotator::NormalizeAxis(DesiredRotation.Yaw);
	DesiredRotation.Roll = 0.f;

	// Accumulate a desired new rotation.
	const float AngleTolerance = 1e-3f;

	if (!CurrentRotation.Equals(DesiredRotation, AngleTolerance)) {
		// PITCH
		if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance)) {
			DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
		}

		// YAW
		if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance)) {
			DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
		}

		// ROLL
		if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance)) {
			DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
		}

		// Set the new rotation.
		DesiredRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));
		movementComponent->MoveUpdatedComponent(FVector::ZeroVector, DesiredRotation, /*bSweep*/ false);
	}
}

FRotator UMovementModule_Walk::GetDeltaRotation(float DeltaTime, FRotator RotationRate) const {
	auto pitch = (RotationRate.Pitch >= 0.f) ? (RotationRate.Pitch * DeltaTime) : 360.f;
	auto yaw = (RotationRate.Yaw >= 0.f) ? (RotationRate.Yaw * DeltaTime) : 360.f;
	auto roll = (RotationRate.Roll >= 0.f) ? (RotationRate.Roll * DeltaTime) : 360.f;
	return FRotator(pitch, yaw, roll);
}

FRotator UMovementModule_Walk::ComputeOrientToMovementRotation(const FRotator& CurrentRotation, float DeltaTime, FRotator& DeltaRotation, FVector movementDirection) const {
	if (movementDirection.SizeSquared() < KINDA_SMALL_NUMBER) {
		return CurrentRotation;
	}
	// Rotate toward direction of acceleration.
	return movementDirection.GetSafeNormal().Rotation();
}
