// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementModule_Base.h"
#include "GameFramework/MovementComponent.h"
#include "ProjectSettingsFacade.h"
#include "MovementModule_Fall.generated.h"

/**
 * 
 */
UCLASS()
class UMovementModule_Fall : public UMovementModule_Base
{
	GENERATED_UCLASS_BODY()
	
public:

	virtual void ApplyMovement(FMovementContext_Eval& context, const FMovementContext_Character& characterContext, FFindFloorResult& CurrentFloor, UMovementComponent* movementComponent) const override;

protected:

	virtual FVector CalcExternalAcceleration(FMovementContext_Eval& context, const FMovementContext_Character& characterContext, FFindFloorResult& CurrentFloor, UMovementComponent* movementComponent) const override {
		return UProjectSettingsFacade::GetGravity(movementComponent->GetOwner()->GetActorLocation());
	}
};
