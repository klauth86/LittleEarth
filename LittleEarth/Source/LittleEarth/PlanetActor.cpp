// Fill out your copyright notice in the Description page of Project Settings.


#include "PlanetActor.h"
#include "LE_GameInstance.h"

APlanetActor::APlanetActor(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {
	PrimaryActorTick.bCanEverTick = true;
}

void APlanetActor::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	switch (PlanetState) {
	case EPlanetState::STABLE:
		Tick_Stable(DeltaSeconds);
		break;

	case EPlanetState::COLLAPSING:
		Tick_Collapsing(DeltaSeconds);
		break;

	default:
		break;
	}
}

void APlanetActor::BeginPlay() {
	Super::BeginPlay();
}

void APlanetActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
}

void APlanetActor::OnPlayerMoved(FVector delta) {

	if (delta.IsNearlyZero()) return;

	FRotator deltaRot;
	deltaRot.Pitch = delta.X * 180;
	deltaRot.Roll = -delta.Y * 180;
	SetActorRotation(deltaRot.Quaternion() * GetActorQuat());

}

void APlanetActor::Tick_Stable(float DeltaSeconds) {
	if (auto instance = ULE_GameInstance::GetInstance()) {
		if (instance->AddTime(-DeltaSeconds) == 0) {
			PlanetState = EPlanetState::COLLAPSING;
		}
	}
}

void APlanetActor::Tick_Collapsing(float DeltaSeconds) {

}