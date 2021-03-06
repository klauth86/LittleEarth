// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup_Base.generated.h"

class UCapsuleComponent;
class UStaticMeshComponent;

/**
 * 
 */
UCLASS()
class LITTLEEARTH_API APickup_Base : public AActor {

	GENERATED_BODY()

public:

	APickup_Base();

protected:

	UPROPERTY(Category = "PickUp", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(Category = "PickUp", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;

public:

	FORCEINLINE UCapsuleComponent* GetCapsuleComponent() { return CapsuleComponent; };

	FORCEINLINE UStaticMeshComponent* GetStaticMeshComponent() { return StaticMeshComponent; };

public:

	void BeginPlay() override;

	void Tick(float DeltaSeconds) override;

	virtual void PickUp();

protected:

	UPROPERTY(EditAnywhere, Category = "PickUp")
	TSubclassOf<AActor> Spawn_VFXClass = nullptr;

	UPROPERTY(EditAnywhere, Category = "PickUp")
	USoundBase* Spawn_SFX = nullptr;

	UPROPERTY(EditAnywhere, Category = "PickUp")
	TSubclassOf<AActor> PickUp_VFXClass;

	UPROPERTY(EditAnywhere, Category = "PickUp")
	USoundBase* PickUp_SFX = nullptr;

	UPROPERTY(EditAnywhere, Category = "PickUp")
	FRotator AngularVelocity;

};
