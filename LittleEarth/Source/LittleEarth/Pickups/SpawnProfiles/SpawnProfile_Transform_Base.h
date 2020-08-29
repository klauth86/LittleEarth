// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SpawnProfile_Transform_Base.generated.h"

UCLASS(BlueprintType, Blueprintable)
class LITTLEEARTH_API USpawnProfile_Transform_Base: public UObject {

	GENERATED_BODY()

public:

	virtual FTransform GetSpawnTransform() { return FTransform::Identity; }

};
