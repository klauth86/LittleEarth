// Fill out your copyright notice in the Description page of Project Settings.


#include "LE_RadialArmComponent.h"
#include "GameFramework/Actor.h"

const FName ULE_RadialArmComponent::SocketName(TEXT("RadialEndpoint"));

ULE_RadialArmComponent::ULE_RadialArmComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	bAutoActivate = true;
	bTickInEditor = true;

	TargetArmLength = 300.0f;
	CameraLagSpeed = 10.f;
}

void ULE_RadialArmComponent::OnRegister() {
	Super::OnRegister();
	UpdateSocketTransform(false, 0.f);
}

void ULE_RadialArmComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateSocketTransform(bEnableCameraLag, DeltaTime);
}

void ULE_RadialArmComponent::UpdateSocketTransform(bool enableCameraLag, float DeltaTime) {
	
	auto owner = GetOwner();
	auto ownerLocation = owner->GetActorLocation();

	const FVector PreviousSocketLocation = SocketLocation;

	FVector DesiredSocketLoc;
	if (Center) {
		auto direction = (ownerLocation - Center->GetActorLocation()).GetSafeNormal();
		DesiredSocketLoc = ownerLocation + direction * TargetArmLength;

	}
	else {
		auto direction =owner->GetActorUpVector();
		DesiredSocketLoc = ownerLocation + direction * TargetArmLength;
	}

	if (enableCameraLag) {
		SocketLocation = FMath::VInterpTo(PreviousSocketLocation, DesiredSocketLoc, DeltaTime, CameraLagSpeed);
	}
	else {
		SocketLocation = DesiredSocketLoc;
	}

	SocketRotation = (ownerLocation - SocketLocation).ToOrientationQuat();
}

FTransform ULE_RadialArmComponent::GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace) const {
	FTransform RelativeTransform(SocketRotation, SocketLocation);

	switch (TransformSpace) {
	case RTS_World:
	{
		return RelativeTransform * GetComponentTransform();
		break;
	}
	case RTS_Actor:
	{
		if (const AActor* Actor = GetOwner()) {
			FTransform SocketTransform = RelativeTransform * GetComponentTransform();
			return SocketTransform.GetRelativeTransform(Actor->GetTransform());
		}
		break;
	}
	case RTS_Component:
	{
		return RelativeTransform;
	}
	}
	return RelativeTransform;
}

bool ULE_RadialArmComponent::HasAnySockets() const {
	return true;
}

void ULE_RadialArmComponent::QuerySupportedSockets(TArray<FComponentSocketDescription>& OutSockets) const {
	new (OutSockets) FComponentSocketDescription(SocketName, EComponentSocketType::Socket);
}

