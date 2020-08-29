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

	void BeginPlay() override;

	virtual void PickUp();

protected:

	TSubclassOf<AActor> Spawn_VFXClass = nullptr;
	USoundBase* Spawn_SFX = nullptr;

	TSubclassOf<AActor> PickUp_VFXClass;
	USoundBase* PickUp_SFX = nullptr;

};
