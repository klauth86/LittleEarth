// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Misc/Guid.h"
#include "PickupSpawner_Base.generated.h"

class USpawnProfile_Pickup_Base;
class USpawnProfile_Transform_Base;

UCLASS()
class LITTLEEARTH_API APickupSpawner_Base : public AActor {

	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = "Pickup Spawner")
		float SpawnRate = 10.f;

	UPROPERTY(EditAnywhere, Category = "Pickup Spawner")
		bool SpawnInfinitely = false;

	UPROPERTY(EditAnywhere, Category = "Pickup Spawner")
		TSubclassOf<USpawnProfile_Pickup_Base> PickupProfileClass;

	UPROPERTY(EditAnywhere, Category = "Pickup Spawner")
		TSubclassOf<USpawnProfile_Transform_Base> TransformProfileClass;

	UPROPERTY(EditAnywhere, Category = "Pickup Spawner") // TODO DISABLE IN EDITOR IF SpawnInfinitely
		uint8 NumToSpawn = 1;

	FGuid GameTaskId;

protected:

	virtual void BeginPlay() override;

public:

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Spawn();

};
