#pragma once

#include "ObjectMacros.h"
#include "MovementContext_Character.generated.h"

USTRUCT(BlueprintType)
struct FMovementContext_Character {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta = (ClampMin = "0", UIMin = "0"))
		float PerchRadiusThreshold;

	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta = (ClampMin = "0", UIMin = "0"))
		float PerchAdditionalHeight;

	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float MaxStepHeight;

	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "90.0", UIMin = "0.0", UIMax = "90.0"))
		float WalkableFloorAngle;

	UPROPERTY(Category = "Character Movement: Walking", VisibleAnywhere)
		float WalkableFloorZ;

	UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite)
		uint8 bMaintainHorizontalGroundVelocity : 1;

	UPROPERTY(Category = "Character Movement: Rotation", EditAnywhere, BlueprintReadWrite)
		uint8 bUseControllerDesiredRotation : 1;

	UPROPERTY(Category = "Character Movement: Rotation", EditAnywhere, BlueprintReadWrite)
		uint8 bOrientRotationToMovement : 1;

	UPROPERTY(Category = "Character Movement: Rotation", EditAnywhere, BlueprintReadWrite)
		FRotator RotationRate;

	UPROPERTY(Category = "Character Movement: Jumping", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Jump Z Velocity", ClampMin = "0", UIMin = "0"))
		float JumpZVelocity = 1000.f;

	UPROPERTY(Category = "Character Movement: Jumping", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Jump Z Velocity", ClampMin = "0", UIMin = "0"))
		float BreakthroughVelocity = 1000.f;

	UPROPERTY(Category = "Character Movement: Jumping", EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "Jump Z Velocity", ClampMin = "0", UIMin = "0"))
		float BreakthroughAngle = -20.f;
};