// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/SpawnProfiles/SpawnProfile_Transform_Base.h"
#include "SpawnProfile_Transform_Sphere.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class LITTLEEARTH_API USpawnProfile_Transform_Sphere : public USpawnProfile_Transform_Base {

	GENERATED_BODY()
	
public:

	virtual FTransform GetSpawnTransform() override;

protected:

	UPROPERTY(EditAnywhere, Category = "Spawn Profile Sphere")
		float MinRadius = 0;

	UPROPERTY(EditAnywhere, Category = "Spawn Profile Sphere")
		float MaxRadius = 1;

	UPROPERTY(EditAnywhere, Category = "Spawn Profile Sphere")
		float MinPhi = 0;

	UPROPERTY(EditAnywhere, Category = "Spawn Profile Sphere")
		float MaxPhi = 360;

	UPROPERTY(EditAnywhere, Category = "Spawn Profile Sphere")
		float MinTheta = 0;

	UPROPERTY(EditAnywhere, Category = "Spawn Profile Sphere")
		float MaxTheta = 180;
};
