// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dicts/PlanetState.h"
#include "Planet_Base.generated.h"

class IPhysActor;

UCLASS()
class LITTLEEARTH_API APlanet_Base : public AActor {

	GENERATED_UCLASS_BODY()

public:

	virtual void Tick(float DeltaSeconds) { 
		Tick_Internal(DeltaSeconds);
	}

protected:

	virtual void Tick_Internal(float DeltaSeconds);

	virtual void AddPhysForceToPhysActor_Internal(float DeltaSeconds, IPhysActor* physActor) {}
};
