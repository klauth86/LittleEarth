// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MovementContext_Eval.h"
#include "ActorWithMovementContext.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UActorWithMovementContext : public UInterface
{
	GENERATED_BODY()
};

class IActorWithMovementContext
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void FillContext(FMovementContext_Eval& context) {}
};
