// Fill out your copyright notice in the Description page of Project Settings.


#include "Planet_Base.h"
#include "Interfaces/PhysActor.h"

APlanet_Base::APlanet_Base(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {
	PrimaryActorTick.bCanEverTick = true;
}

void APlanet_Base::Tick_Internal(float DeltaSeconds) {
	for (auto physActor : IPhysActor::GetAllInstances()) {
		AddPhysForceToPhysActor_Internal(DeltaSeconds, physActor);
	}
}