// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Movement/MovementContext_Eval.h"
#include "InputModifier_Base.generated.h"

class IOwnerMulticasterComponentBase;

UCLASS()
class UInputModifier_Base : public UObject {

	GENERATED_UCLASS_BODY()

public:

	virtual void Apply(IOwnerMulticasterComponentBase* multicaster, FMovementContext_Eval& context) const {}
};