// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupSpawner_Base.h"
#include "GameTasks/GameTaskService.h"
#include "SpawnProfiles/SpawnProfile_Pickup_Base.h"
#include "Pickup_Base.h"
#include "Engine/World.h"
#include "LE_Common.h"

void APickupSpawner_Base::BeginPlay() {
	Super::BeginPlay();
	auto numToExecute = SpawnInfinitely ? LE_Common::EXECUTE_INFINIT_TIMES : NumToSpawn;
	GameTaskId = GameTaskService::AddGameTask(SpawnRate, this, &APickupSpawner_Base::Spawn, numToExecute);
}

void APickupSpawner_Base::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	GameTaskService::RemoveGameTask(GameTaskId);
}

void APickupSpawner_Base::Spawn() {
	if (SpawnProfileClass) {
		auto spawnProfile = SpawnProfileClass->GetDefaultObject<USpawnProfile_Pickup_Base>();
		auto classToSpawn = spawnProfile->GetClassToSpawn();
		GetWorld()->SpawnActor<APickup_Base>(classToSpawn, GetSpawnTransform());
	}
}