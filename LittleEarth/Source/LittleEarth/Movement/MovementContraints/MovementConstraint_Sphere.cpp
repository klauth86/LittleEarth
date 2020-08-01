// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementConstraint_Sphere.h"
#include "GameFramework/Actor.h"
#include "Components/OwnerMulticasterComponentBase.h"
#include "Utils/LogManager.h"

FPlayerMovedEvent UMovementConstraint_Sphere::OnPlayerMoved;

void UMovementConstraint_Sphere::Apply(IOwnerMulticasterComponentBase* multicaster, FMovementContext_Eval& context, const FMovementContext_Character& characterContext, const UMovementModule_Base* module) const {
	auto owner = multicaster->GetSelfAsActorComponent()->GetOwner();
	auto location = owner->GetActorLocation();
	auto locationSize = location.Size();

	context.HasOverrideVelocity2D = true;

	OnPlayerMoved.Broadcast(context.GetPrimaryDelta() / locationSize);	
}

void UMovementConstraint_Sphere::Advance(IOwnerMulticasterComponentBase* multicaster, FMovementContext_Eval& context, const FMovementContext_Character& characterContext, const UMovementModule_Base* module) const {

}