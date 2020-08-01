// Fill out your copyright notice in the Description page of Project Settings.

#include "PawnMovementComponent_Base.h"
#include "GameFramework/Pawn.h"

#include "Movement/InputModifiers/InputModifier_Base.h"
#include "Movement/MovementModules/MovementModule_Base.h"
#include "Movement/MovementModifiers/MovementModifier_Base.h"
#include "Movement/MovementContraints/MovementConstraint_Base.h"
#include "Movement/ActorWithMovementContext.h"
#include "Movement/MovementContext_Eval.h"

#include "Components/CapsuleComponent.h"

#include "Utils/FloorHelper.h"

FMovementModeChanged UPawnMovementComponent_Base::OnMovementModeChanged;

UPawnMovementComponent_Base::UPawnMovementComponent_Base(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	
	CharacterContext.MaxStepHeight = 45.0f;
	CharacterContext.PerchRadiusThreshold = 0.0f;
	CharacterContext.PerchAdditionalHeight = 40.f;

	CharacterContext.WalkableFloorZ = 0.71f;
	CharacterContext.WalkableFloorAngle = 45.f;

	CharacterContext.bMaintainHorizontalGroundVelocity = true;

	CharacterContext.RotationRate = FRotator(0.f, 360.0f, 0.0f);
	CharacterContext.bUseControllerDesiredRotation = false;

	movementMode = EMovementMode::MOVE_Walking;
	MovementIsEnabled = true;
}

const float END_JUMP_ANIM_TIME_DURATION = 0.5f;	// TODO THINK IF IT CAN BE EXTRACTED FROM STATE MACHINE

void UPawnMovementComponent_Base::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {

	//LogManager::LogWarning(TEXT("%s"), *GetOwner()->GetName());

	if (!MovementIsEnabled)
		return;

	if (!HasValidData() || ShouldSkipUpdate(DeltaTime)) {
		return;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!HasValidData()) {
		return;
	}

	// We don't update if simulating physics (eg ragdolls)
	if (UpdatedComponent->IsSimulatingPhysics()) {
		return;
	}

	UpdateFloor(CurrentFloor, CapsuleComponent, CharacterContext);	// Update current floor

	UpdateMode();

	FMovementContext_Eval context;

	// Fill context
	if (auto actorWithContext = GetOwnerAs<IActorWithMovementContext>()) {
		actorWithContext->FillContext(context);
	}

	for (auto& modifierClass : InputModifiers) {	// Apply modifiers to velocity
		modifierClass->GetDefaultObject<UInputModifier_Base>()->Apply(this, context);
	}

	auto subtickDeltaTime = DeltaTime / EvalSubdivision;
	for (size_t i = 0; i < EvalSubdivision; i++) {

		context.Velocity = Velocity;
		context.DeltaTime = subtickDeltaTime;

		////LogManager::LogWarning(TEXT("%s move(%s) with velocity BEFORE %s"), *PawnOwner->GetName(), *GetEnumValueAsString("EMovementMode", movementMode),
		//	*Velocity.ToString());

		auto movementModule = MovementModules[movementMode]->GetDefaultObject<UMovementModule_Base>();
		
		movementModule->CalcVelocity(context, CharacterContext, CurrentFloor, this);	// Calc velocity		
		////LogManager::LogWarning(TEXT("%s move(%s) with velocity CALC %s"), *PawnOwner->GetName(), *GetEnumValueAsString("EMovementMode", movementMode),
		//	*Velocity.ToString());


		for (auto& constraintClass : MovementConstraints) {	// Apply constraints
			constraintClass->GetDefaultObject<UMovementConstraint_Base>()->Apply(this, context, CharacterContext, movementModule);
		}
		////LogManager::LogWarning(TEXT("%s move(%s) with velocity CONSTRAINTs %s"), *PawnOwner->GetName(), *GetEnumValueAsString("EMovementMode", movementMode),
		//	*Velocity.ToString());

		for (auto& modifierClass : MovementModifiers) {	// Apply modifiers to velocity
			modifierClass->GetDefaultObject<UMovementModifier_Base>()->Apply(this, context, CharacterContext, movementModule);
		}
		////LogManager::LogWarning(TEXT("%s move(%s) with velocity MODIFs %s"), *PawnOwner->GetName(), *GetEnumValueAsString("EMovementMode", movementMode),
		//	*Velocity.ToString());


		movementModule->ApplyMovement(context, CharacterContext, CurrentFloor, this);	// Movement eval context can be changed by this (for example, blocking wall stop our movement, etc.)
		////LogManager::LogWarning(TEXT("%s move(%s) with velocity AFTER %s"), *PawnOwner->GetName(), *GetEnumValueAsString("EMovementMode", movementMode),
		//	*Velocity.ToString());
		
		movementModule->Adjust(context, CharacterContext, CurrentFloor, this);			// That's why we advance constraints after that and update Velocity after that either

		for (auto& constraintClass : MovementConstraints) {	// Advance constraints
			constraintClass->GetDefaultObject<UMovementConstraint_Base>()->Advance(this, context, CharacterContext, movementModule);
		}

		Velocity = context.Velocity;	// Update velocity
	}

	if (Velocity.SizeSquared2D() > 0)
		OnReachLocation.Broadcast();
}

bool UPawnMovementComponent_Base::HasValidData() const {
	return UpdatedComponent;
}

void UPawnMovementComponent_Base::SetUpdatedComponent(USceneComponent* updatedComponent) {
	UNavMovementComponent::SetUpdatedComponent(updatedComponent);
	CapsuleComponent = Cast<UCapsuleComponent>(updatedComponent);
}

void UPawnMovementComponent_Base::AddJumpImpulse() {
	Velocity.Z += CharacterContext.JumpZVelocity;
	SetMovementMode(EMovementMode::MOVE_Falling);
}

void UPawnMovementComponent_Base::AddBreakthroughImpulse() {
	auto owner = GetOwner();
	auto forward = owner->GetActorForwardVector();
	auto right = owner->GetActorRightVector();
	Velocity += (forward * CharacterContext.BreakthroughVelocity).RotateAngleAxis(CharacterContext.BreakthroughAngle, right);
	SetMovementMode(EMovementMode::MOVE_Falling);
}

void UPawnMovementComponent_Base::UpdateMode() {
	
	// TODO GRAVITY
	auto landingHeight = FMath::Abs(GetFallingDeltaZByTime(9, END_JUMP_ANIM_TIME_DURATION));

	if (movementMode == EMovementMode::MOVE_Walking) {
		if (!CurrentFloor.IsWalkableFloor()) {
			SetMovementMode(EMovementMode::MOVE_Falling);
			OnStartFallingOffGround.Broadcast();
		}
	}
	else if (movementMode == EMovementMode::MOVE_Falling && CurrentFloor.IsWalkableFloor()) {
		float dist = CurrentFloor.GetDistanceToFloor();

		// We can start landing
		if (dist < landingHeight && FVector::DotProduct(CurrentFloor.HitResult.Normal, Velocity) <= 0) {
			
			OnStartLandingAtGround.Broadcast();
			
			// We can end landing
			if (dist < UCharacterMovementComponent::MIN_FLOOR_DIST) {
				SetMovementMode(EMovementMode::MOVE_Walking);
				OnEndLandingAtGround.Broadcast();
			}
		}
	}
}

bool UPawnMovementComponent_Base::IsFalling() const {
	return (movementMode == MOVE_Falling) && UpdatedComponent;
}