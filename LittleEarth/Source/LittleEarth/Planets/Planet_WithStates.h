// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Planets/Planet_Base.h"
#include "Dicts/PlanetState.h"
#include "Planet_WithStates.generated.h"

/**
 * 
 */
UCLASS()
class LITTLEEARTH_API APlanet_WithStates : public APlanet_Base {

	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, Category = "Planet With States")
		TEnumAsByte<EPlanetState> PlanetState = EPlanetState::STABLE;



	virtual void Tick_Internal(float DeltaSeconds) override;

	void Tick_Collapsing(float DeltaSeconds);



	virtual void AddPhysForceToPhysActor_Internal(float DeltaSeconds, IPhysActor* physActor) override;

	void AddPhysForceToPhysActor_Internal_Stable(float DeltaSeconds, IPhysActor* physActor);

	void AddPhysForceToPhysActor_Internal_Collapsing(float DeltaSeconds, IPhysActor* physActor);
};
