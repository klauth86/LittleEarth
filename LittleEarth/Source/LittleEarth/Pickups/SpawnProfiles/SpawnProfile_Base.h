// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SpawnProfile_Base.generated.h"

class APickup_Base;

UCLASS()
class LITTLEEARTH_API USpawnProfile_Base: public UObject {

	GENERATED_BODY()

public:

	virtual TSubclassOf<APickup_Base> GetClassToSpawn() { return nullptr; }

};
