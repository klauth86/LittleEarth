// Fill out your copyright notice in the Description page of Project Settings.


#include "Planet_WithStates.h"
#include "Interfaces/PhysActor.h"
#include "LE_Common.h"
#include "Components/PrimitiveComponent.h"

void APlanet_WithStates::Tick_Internal(float DeltaSeconds) {
	Super::Tick_Internal(DeltaSeconds);

	switch (PlanetState) {
	case EPlanetState::STABLE:

		break;

	case EPlanetState::COLLAPSING:
		Tick_Collapsing(DeltaSeconds);
		break;

	default:
		break;
	}
}

void APlanet_WithStates::Tick_Collapsing(float DeltaSeconds) {} // TODO



void APlanet_WithStates::AddPhysForceToPhysActor_Internal(float DeltaSeconds, IPhysActor* physActor) {
	switch (PlanetState) {
	case EPlanetState::STABLE:
		AddPhysForceToPhysActor_Internal_Stable(DeltaSeconds, physActor);
		break;

	case EPlanetState::COLLAPSING:
		AddPhysForceToPhysActor_Internal_Collapsing(DeltaSeconds, physActor);
		break;

	default:
		break;
	}
}

void APlanet_WithStates::AddPhysForceToPhysActor_Internal_Stable(float DeltaSeconds, IPhysActor* physActor) {
	auto primitiveComponent = physActor->GetPrimitiveComponent();
	auto mass = primitiveComponent->GetMass();
	physActor->GetPrimitiveComponent()->AddRadialForce(
		GetActorLocation(),
		LE_Common::PLANET_RADIAL_FIELD_MAX_RADIUS,
		-mass * LE_Common::EARTH_G * PlanetContext.PlanetG,
		ERadialImpulseFalloff::RIF_MAX);
}

void APlanet_WithStates::AddPhysForceToPhysActor_Internal_Collapsing(float DeltaSeconds, IPhysActor* physActor) {

}