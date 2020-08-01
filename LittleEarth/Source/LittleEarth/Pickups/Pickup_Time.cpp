// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup_Time.h"
#include "LE_GameInstance.h"

void APickup_Time::PickUp() {

	Super::PickUp();

	if (auto instance = ULE_GameInstance::GetInstance()) {
		instance->AddTime(TimeToAdd);
	}
}