#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Movement/MovementContext_Character.h"
#include "Engine/EngineTypes.h"

extern FVector ComputeGroundMovementDelta(const FVector& Delta, const FHitResult& RampHit, const bool bHitFromLineTrace,
	const FMovementContext_Character& characterContext);

extern bool CanStepUp(const FHitResult& Hit, APawn* pawn);

extern bool IsWithinEdgeTolerance(const FVector& capsuleLocation, const FVector& testImpactPoint, const float CapsuleRadius);

extern bool IsWalkable(const FHitResult& Hit, const FMovementContext_Character& characterContext);

extern void UpdateFloor(FFindFloorResult& OutFloorResult, const UCapsuleComponent* capsuleComponent, const FMovementContext_Character& characterContext);

extern void ComputeFloorDist(FFindFloorResult& OutFloorResult, const UCapsuleComponent* capsuleComponent, const FMovementContext_Character& characterContext, 
	float distanceToTest, FVector start);

extern void ComputeFloorDist(FFindFloorResult& OutFloorResult, float PawnHalfHeight, float PawnRadius, const UCapsuleComponent* capsuleComponent,
	const FMovementContext_Character& characterContext, float sweepDistanceToTest, float lineDistanceToTest, FVector start);