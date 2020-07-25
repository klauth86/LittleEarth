#include "FloorHelper.h"

// Add Z component ro horizontal movement with triangle tg(alpha)
FVector ComputeGroundMovementDelta(const FVector& Delta, const FHitResult& RampHit, const bool bHitFromLineTrace, const FMovementContext_Character& characterContext) {
	const FVector FloorNormal = RampHit.ImpactNormal;
	const FVector ContactNormal = RampHit.Normal;

	if (FloorNormal.Z < (1.f - KINDA_SMALL_NUMBER) && 
		FloorNormal.Z > KINDA_SMALL_NUMBER && 
		ContactNormal.Z > KINDA_SMALL_NUMBER &&
		!bHitFromLineTrace &&
		IsWalkable(RampHit, characterContext)) {

		// Compute a vector that moves parallel to the surface, by projecting the horizontal movement direction onto the ramp.
		const float FloorDotDelta = (FloorNormal | Delta);
		FVector RampMovement(Delta.X, Delta.Y, -FloorDotDelta / FloorNormal.Z);

		if (characterContext.bMaintainHorizontalGroundVelocity) {
			return RampMovement;
		}
		else {
			return RampMovement.GetSafeNormal() * Delta.Size();
		}
	}
	return Delta;
}

// Check common possibility for object to be base for step
bool CanStepUp(const FHitResult& Hit, APawn* pawn) {
	if (!Hit.IsValidBlockingHit()) {
		return false;
	}
	
	// No component for "fake" hits when we are on a known good base.
	const UPrimitiveComponent* HitComponent = Hit.Component.Get();
	if (!HitComponent) {
		return true;
	}

	if (!HitComponent->CanCharacterStepUp(pawn)) {
		return false;
	}

	// No actor for "fake" hits when we are on a known good base.
	const AActor* HitActor = Hit.GetActor();
	if (!HitActor) {
		return true;
	}

	if (!HitActor->CanBeBaseForCharacter(pawn)) {
		return false;
	}

	return true;
}

// Check if testPoint is inside capsule radius
bool IsWithinEdgeTolerance(const FVector& capsuleLocation, const FVector& testImpactPoint, const float CapsuleRadius) {
	const float distanceSqr = (testImpactPoint - capsuleLocation).SizeSquared2D();
	const float reducedRadius = FMath::Max(UCharacterMovementComponent::SWEEP_EDGE_REJECT_DISTANCE + KINDA_SMALL_NUMBER,
		CapsuleRadius - UCharacterMovementComponent::SWEEP_EDGE_REJECT_DISTANCE); // Reject if in SWEEP_EDGE_REJECT_DISTANCE interval from radius
	const float reducedRadiusSqr = reducedRadius * reducedRadius;
	return distanceSqr < reducedRadiusSqr;
}

// Define angle of the surface and compare with Character property
bool IsWalkable(const FHitResult& Hit, const FMovementContext_Character& characterContext) {

	if (!Hit.IsValidBlockingHit()) {	// No hit, or starting in penetration
		return false;
	}

	if (Hit.ImpactNormal.Z < KINDA_SMALL_NUMBER) {	// Never walk up vertical surfaces.
		return false;
	}

	float TestWalkableZ = characterContext.WalkableFloorZ;

	const UPrimitiveComponent* HitComponent = Hit.Component.Get();		// See if this component overrides the walkable floor z.
	if (HitComponent) {
		const FWalkableSlopeOverride& SlopeOverride = HitComponent->GetWalkableSlopeOverride();
		TestWalkableZ = SlopeOverride.ModifyWalkableFloorZ(TestWalkableZ);
	}

	if (Hit.ImpactNormal.Z < TestWalkableZ) {		// Can't walk on this surface if it is too steep.
		return false;
	}

	return true;
}

// Update floor with ComputeFloorDist method
void UpdateFloor(FFindFloorResult& OutFloorResult, const UCapsuleComponent* capsuleComponent, const FMovementContext_Character& characterContext) {

	OutFloorResult.Clear(); // Clear prev results anyway

	if (!capsuleComponent || !capsuleComponent->IsQueryCollisionEnabled()) {	// No collision, no floor...
		return;
	}

	auto capsuleLocation = capsuleComponent->GetComponentLocation();

	auto heightCheckAdjust = UCharacterMovementComponent::MAX_FLOOR_DIST;
	float traceDist = characterContext.MaxStepHeight + heightCheckAdjust;

	ComputeFloorDist(OutFloorResult, capsuleComponent, characterContext, traceDist, capsuleLocation);
}

void ComputeFloorDist(FFindFloorResult& OutFloorResult, const UCapsuleComponent* capsuleComponent, const FMovementContext_Character& characterContext,
	float distanceToTest, FVector start) {

	float PawnRadius, PawnHalfHeight;
	capsuleComponent->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);

	ComputeFloorDist(OutFloorResult, PawnHalfHeight, PawnRadius, capsuleComponent, characterContext, distanceToTest, distanceToTest, start);
}

// Compute distance to floor with two methods - capsule sweep and line trace
void ComputeFloorDist(FFindFloorResult& OutFloorResult, float PawnHalfHeight, float PawnRadius, const UCapsuleComponent* capsuleComponent,
	const FMovementContext_Character& characterContext, float sweepDistanceToTest, float lineDistanceToTest, FVector start) {
	
	auto owner = capsuleComponent->GetOwner();
	auto world = owner->GetWorld();

	// We require the sweep distance to be >= the line distance, otherwise the HitResult can't be interpreted as the sweep result.
	if (sweepDistanceToTest < lineDistanceToTest) {
		ensure(sweepDistanceToTest >= lineDistanceToTest);
		return;
	}

	bool bBlockingHit = false;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ComputeFloorDist), false, owner);
	FCollisionResponseParams ResponseParam;
	capsuleComponent->InitSweepCollisionParams(QueryParams, ResponseParam);
	const ECollisionChannel CollisionChannel = capsuleComponent->GetCollisionObjectType();

	// Can do sweep test if nonzero distance and nonzero radius
	if (sweepDistanceToTest > 0.f && PawnRadius > 0.f) {
		// Use smaller shape to check surfaces

		const float ShrinkScale = 0.9f;
		const float ShrinkScaleOverlap = 0.1f;

		float ShrinkHeight = (PawnHalfHeight - PawnRadius) * (1.f - ShrinkScale);
		FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(PawnRadius, PawnHalfHeight - ShrinkHeight);

		float TraceDist = sweepDistanceToTest + ShrinkHeight; // correction due to smaller shape sweep

		FHitResult Hit(1.f);
		bBlockingHit = world->SweepSingleByChannel(Hit, start, start + FVector(0.f, 0.f, -TraceDist), FQuat::Identity, CollisionChannel,
			CapsuleShape, QueryParams, ResponseParam);

		if (bBlockingHit) {

			auto isWithinRadius = IsWithinEdgeTolerance(start, Hit.ImpactPoint, PawnRadius); // TODO MAYBE USE SOME MULTIPLIER IN THE FUTURE

			if (Hit.bStartPenetrating || !isWithinRadius) {

				CapsuleShape.Capsule.Radius = FMath::Max(0.f, CapsuleShape.Capsule.Radius - UCharacterMovementComponent::SWEEP_EDGE_REJECT_DISTANCE - KINDA_SMALL_NUMBER);

				if (!CapsuleShape.IsNearlyZero()) {

					ShrinkHeight = (PawnHalfHeight - PawnRadius) * (1.f - ShrinkScaleOverlap);
					CapsuleShape.Capsule.HalfHeight = FMath::Max(PawnHalfHeight - ShrinkHeight, CapsuleShape.Capsule.Radius);

					TraceDist = sweepDistanceToTest + ShrinkHeight;

					Hit.Reset(1.f, false);
					bBlockingHit = world->SweepSingleByChannel(Hit, start, start + FVector(0.f, 0.f, -TraceDist), FQuat::Identity, CollisionChannel,
						CapsuleShape, QueryParams, ResponseParam);
				}
			}

			// Reduce hit distance by ShrinkHeight because we shrank the capsule for the trace.
			// We allow negative distances here, because this allows us to pull out of penetrations.
			const float MaxPenetrationAdjust = FMath::Max(UCharacterMovementComponent::MAX_FLOOR_DIST, PawnRadius);
			const float SweepResult = FMath::Max(-MaxPenetrationAdjust, Hit.Time * TraceDist - ShrinkHeight);

			OutFloorResult.SetFromSweep(Hit, SweepResult, false);
			if (Hit.IsValidBlockingHit() && IsWalkable(Hit, characterContext)) {
				if (SweepResult <= sweepDistanceToTest) {	// Hit within test distance.
					OutFloorResult.bWalkableFloor = true;
					return;
				}
			}
		}
	}


	// Since we require a longer sweep than line trace, we don't want to run the line trace if the sweep missed everything.
	// We do however want to try a line trace if the sweep was stuck in penetration.
	if (!OutFloorResult.bBlockingHit && !OutFloorResult.HitResult.bStartPenetrating) {
		OutFloorResult.FloorDist = sweepDistanceToTest;
		return;
	}




	// Can do Line trace test if nonzero distance
	if (lineDistanceToTest > 0.f) {
		const float ShrinkHeight = PawnHalfHeight;
		const FVector LineTraceStart = start;

		const float TraceDist = lineDistanceToTest + PawnHalfHeight;

		const FVector Down = FVector(0.f, 0.f, -TraceDist);
		QueryParams.TraceTag = SCENE_QUERY_STAT_NAME_ONLY(FloorLineTrace);

		FHitResult Hit(1.f);
		bBlockingHit = world->LineTraceSingleByChannel(Hit, LineTraceStart, LineTraceStart + Down, CollisionChannel, QueryParams, ResponseParam);

		if (bBlockingHit) {
			if (Hit.Time > 0.f) {

				// Reduce hit distance by ShrinkHeight because we started the trace higher than the base.
				// We allow negative distances here, because this allows us to pull out of penetrations.
				const float MaxPenetrationAdjust = FMath::Max(UCharacterMovementComponent::MAX_FLOOR_DIST, PawnRadius);
				const float LineResult = FMath::Max(-MaxPenetrationAdjust, Hit.Time * TraceDist - ShrinkHeight);

				OutFloorResult.bBlockingHit = true;
				if (LineResult <= lineDistanceToTest && IsWalkable(Hit, characterContext)) {
					OutFloorResult.SetFromLineTrace(Hit, OutFloorResult.FloorDist, LineResult, true);
					return;
				}
			}
		}
	}

	// No hits were acceptable.
	OutFloorResult.bWalkableFloor = false;
	OutFloorResult.FloorDist = lineDistanceToTest;
}