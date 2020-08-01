// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dicts/PlanetState.h"
#include "PlanetActor.generated.h"

UCLASS()
class LITTLEEARTH_API APlanetActor : public AActor {

	GENERATED_UCLASS_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Planet Actor")
		TEnumAsByte<EPlanetState> PlanetState;

public:

	virtual void Tick(float DeltaSeconds);

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
		void OnPlayerMoved(FVector delta);

	void Tick_Stable(float DeltaSeconds);

	void Tick_Collapsing(float DeltaSeconds);

};
