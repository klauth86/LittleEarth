// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SpawnProfile_Pickup_Base.generated.h"

class APickup_Base;

UCLASS(BlueprintType)
class LITTLEEARTH_API USpawnProfile_Pickup_Base: public UObject {

	GENERATED_BODY()

public:

	virtual TSubclassOf<APickup_Base> GetClassToSpawn() { return nullptr; }

};
