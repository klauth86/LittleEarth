// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Movement/MovementContext_Eval.h"
#include "Movement/MovementContext_Character.h"
#include "MovementConstraint_Base.generated.h"

class IOwnerMulticasterComponentBase;
class UMovementModule_Base;

UCLASS()
class UMovementConstraint_Base : public UObject {

	GENERATED_UCLASS_BODY()

public:

	virtual void Apply(IOwnerMulticasterComponentBase* multicaster, FMovementContext_Eval& context, const FMovementContext_Character& characterContext, const UMovementModule_Base* module) const {}

	virtual void Advance(IOwnerMulticasterComponentBase* multicaster, FMovementContext_Eval& context, const FMovementContext_Character& characterContext, const UMovementModule_Base* module) const {}
};