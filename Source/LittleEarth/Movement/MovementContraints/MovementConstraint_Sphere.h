// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Movement/MovementContraints/MovementConstraint_Base.h"
#include "MovementConstraint_Sphere.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerMovedEvent, FVector, delta);

UCLASS()
class LITTLEEARTH_API UMovementConstraint_Sphere : public UMovementConstraint_Base {

	GENERATED_BODY()
	
public:

	static FPlayerMovedEvent OnPlayerMoved;

	virtual void Apply(IOwnerMulticasterComponentBase* multicaster, FMovementContext_Eval& context, const FMovementContext_Character& characterContext, const UMovementModule_Base* module) const override;

	virtual void Advance(IOwnerMulticasterComponentBase* multicaster, FMovementContext_Eval& context, const FMovementContext_Character& characterContext, const UMovementModule_Base* module) const override;
};
