// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/SpawnProfiles/SpawnProfile_Pickup_Base.h"
#include "SpawnProfile_Pickup_Random.generated.h"

class APickup_Base;

/**
 * 
 */
UCLASS()
class LITTLEEARTH_API USpawnProfile_Pickup_Random : public USpawnProfile_Pickup_Base {

	GENERATED_BODY()
	
public:

	virtual TSubclassOf<APickup_Base> GetClassToSpawn() override { 
		if (auto num = PickupsToSpawn.Num()) {
			return PickupsToSpawn[FMath::Rand() % num];
		}
		return nullptr;
; }

protected:

	UPROPERTY(EditAnywhere, Category = "Spawn Profile Random")
	TArray<TSubclassOf<APickup_Base>> PickupsToSpawn;

};
