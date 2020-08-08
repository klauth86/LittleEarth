// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup_Base.generated.h"

/**
 * 
 */
UCLASS()
class LITTLEEARTH_API APickup_Base : public AActor {

	GENERATED_BODY()

public:
	
	virtual void PickUp() {	

		Destroy();
		// TODO ADD SFX TO PLAY
		// TODO ADD VFX TO PLAY

	}

};
