// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PhysActor.generated.h"

class UPrimitiveComponent;

UINTERFACE(MinimalAPI)
class UPhysActor : public UInterface {

	GENERATED_BODY()

};

class LITTLEEARTH_API IPhysActor {

	GENERATED_BODY()

public:

	virtual UPrimitiveComponent* GetPrimitiveComponent() const { return nullptr; }

	static void AddInstance(IPhysActor* physActor) { AllInstances.Add(physActor); }

	static void RemoveInstance(IPhysActor* physActor) { AllInstances.Remove(physActor); }

	static const TArray<IPhysActor*>& GetAllInstances() { return AllInstances; }

protected:

	static TArray<IPhysActor*> AllInstances;
};
