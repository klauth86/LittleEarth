#pragma once

#include "UObject/Object.h"
#include "ObjectMacros.h"
#include "Movement/MovementContext_Eval.h"
#include "Movement/MovementContext_Character.h"
#include "MovementModule_Base.generated.h"

struct FFindFloorResult;
class UMovementComponent;

UCLASS()
class UMovementModule_Base : public UObject {

	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(Category = Movement, EditAnywhere)
		float MaxVelocity;

	UPROPERTY(Category = Movement, EditAnywhere)
		float MaxAcceleration;

	UPROPERTY(Category = Movement, EditAnywhere)
		float Friction;

	UPROPERTY(Category = Movement, EditAnywhere)
		float BrakingDeceleration;

public:

	virtual void CalcVelocity(FMovementContext_Eval& context, const FMovementContext_Character& characterContext, FFindFloorResult& CurrentFloor, UMovementComponent* movementComponent) const;

	virtual void ApplyMovement(FMovementContext_Eval& context, const FMovementContext_Character& characterContext, FFindFloorResult& CurrentFloor, UMovementComponent* movementComponent) const {}

	virtual void Adjust(const FMovementContext_Eval& context, const FMovementContext_Character& characterContext, FFindFloorResult& CurrentFloor, UMovementComponent* movementComponent) const {}

protected:

	virtual FVector CalcInputAcceleration(const FMovementContext_Eval& context, const FMovementContext_Character& characterContext, FFindFloorResult& CurrentFloor, UMovementComponent* movementComponent) const;

	virtual FVector CalcExternalAcceleration(FMovementContext_Eval& context, const FMovementContext_Character& characterContext, FFindFloorResult& CurrentFloor, UMovementComponent* movementComponent) const  {
		return FVector::ZeroVector;
	}

	virtual void OnValidBlockingHit(FMovementContext_Eval& context, FHitResult& inHit) const {
		context.RemoveProjectionFromMovement(inHit.Normal);
	}
};