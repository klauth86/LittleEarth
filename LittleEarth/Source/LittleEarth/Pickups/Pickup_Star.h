// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup_Base.h"
#include "Pickup_Star.generated.h"

UCLASS()
class LITTLEEARTH_API APickup_Star : public APickup_Base {
	GENERATED_BODY()
	
public:
	
	virtual void PickUp() override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup Item")
		int StarsToPick;

};
