// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup_Star.h"
#include "LE_GameInstance.h"

void APickup_Star::PickUp() {

	Super::PickUp();

	if (auto instance = ULE_GameInstance::GetInstance()) {
		instance->AddStars(StarsToPick);
	}
}