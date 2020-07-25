// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Movement/MovementContext_Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "OwnerMulticasterComponentBase.h"
#include "Engine/EngineTypes.h"
#include "PawnMovementComponent_Base.generated.h"

class UInputModifier_Base;
class UMovementModule_Base;
class UMovementModifier_Base;
class UMovementConstraint_Base;
class UPawnMovementComponent_Base;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMovementModeChanged, UPawnMovementComponent_Base*, sender);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartFallingOffGround);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartLandingAtGround);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndLandingAtGround);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReachLocationEvent);

UCLASS()
class UPawnMovementComponent_Base : public UPawnMovementComponent, public IOwnerMulticasterComponentBase {

	GENERATED_UCLASS_BODY()

protected:

	EMovementMode movementMode;

	UPROPERTY(Category = Movement, EditAnywhere, BlueprintReadOnly)
		TArray<TSubclassOf<UInputModifier_Base>> InputModifiers;

	UPROPERTY(Category = Movement, EditAnywhere, BlueprintReadOnly)
	TMap<TEnumAsByte<EMovementMode>, TSubclassOf<UMovementModule_Base>> MovementModules;

	UPROPERTY(Category = Movement, EditAnywhere, BlueprintReadOnly)
		TArray<TSubclassOf<UMovementModifier_Base>> MovementModifiers;

	UPROPERTY(Category = Movement, EditAnywhere, BlueprintReadOnly)
		TArray<TSubclassOf<UMovementConstraint_Base>> MovementConstraints;

	UPROPERTY(Category = Movement, VisibleInstanceOnly, BlueprintReadOnly)
		FFindFloorResult CurrentFloor;

	UCapsuleComponent* CapsuleComponent; // TODO Maybe we can fix it with the same approach as with Owners

public:

	UPROPERTY(Category = "Character Movement", EditAnywhere, meta = (ShowOnlyInnerProperties))
		FMovementContext_Character CharacterContext;

	UPROPERTY(Category = "Character Movement", EditAnywhere, BlueprintReadWrite)
		uint8 MovementIsEnabled : 1;

	UPROPERTY(Category = "Evaluation Subdivision", EditAnywhere, BlueprintReadWrite)
		uint8 EvalSubdivision = 1;

public:

	static FMovementModeChanged OnMovementModeChanged;

	FReachLocationEvent OnReachLocation;

	FStartFallingOffGround OnStartFallingOffGround;

	FStartLandingAtGround OnStartLandingAtGround;

	FEndLandingAtGround OnEndLandingAtGround;

	EMovementMode GetMovementMode() { return movementMode; }

	TSubclassOf<UMovementModule_Base> GetCurrentMovementModule() { return MovementModules[movementMode]; }

	void SetMovementMode(EMovementMode newMode) {
		movementMode = newMode;
		OnMovementModeChanged.Broadcast(this);
	}

	void AddJumpImpulse();

	void AddBreakthroughImpulse();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual bool HasValidData() const;

	virtual void SetUpdatedComponent(USceneComponent* updatedComponent) override;

	virtual bool IsFalling() const override;

protected:

	void UpdateMode();

	FORCEINLINE float GetFallingDeltaZByTime(float gravityZAcceleration, float time) const {
		return gravityZAcceleration * time * time / 2 + Velocity.Z * time;
	}

	virtual UActorComponent* GetSelfAsActorComponent() override { return this; }
};
