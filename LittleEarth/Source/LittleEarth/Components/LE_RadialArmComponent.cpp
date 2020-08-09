// Fill out your copyright notice in the Description page of Project Settings.


#include "LE_RadialArmComponent.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"

const FName ULE_RadialArmComponent::SocketName(TEXT("RadialEndpoint"));

ULE_RadialArmComponent::ULE_RadialArmComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	bAutoActivate = true;
	bTickInEditor = true;

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
	
	if (auto owner = GetOwner()) {
		
		auto ownerLocation = owner->GetActorLocation();
		auto ownerRotation = owner->GetActorRotation();		

		const FVector PreviousSocketLocation = SocketLocation;

		auto size2D = ownerLocation.Z * ownerLocation.Z + ownerLocation.Y * ownerLocation.Y;
		auto size = ownerLocation.Size();

		auto cosAlpha = size2D ? ownerLocation.Y / size2D : 1;
		auto sinAlpha = size2D ? ownerLocation.Z / size2D : 0;
		auto cosTheta = ownerLocation.X / size;
		auto sinTheta = FMath::Sqrt(1 - cosTheta * cosTheta);

		FVector DesiredSocketLoc = ownerLocation +
			FVector(
				cosTheta * Offset.Y - sinTheta * Offset.Z,
				sinTheta * cosAlpha * Offset.Y + cosTheta * cosAlpha * Offset.Z - sinTheta * sinAlpha * Offset.X,
				sinTheta* sinAlpha * Offset.Y + cosTheta * sinAlpha * Offset.Z + sinTheta * cosAlpha * Offset.X
				);

		if (enableCameraLag) {
			SocketLocation = FMath::VInterpTo(PreviousSocketLocation, DesiredSocketLoc, DeltaTime, CameraLagSpeed);
		}
		else {
			SocketLocation = DesiredSocketLoc;
		}

		SocketRotation = (ownerLocation - SocketLocation).ToOrientationQuat();
	}
}

FTransform ULE_RadialArmComponent::GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace) const {
	return FTransform(SocketRotation, SocketLocation);
}

bool ULE_RadialArmComponent::HasAnySockets() const {
	return true;
}

void ULE_RadialArmComponent::QuerySupportedSockets(TArray<FComponentSocketDescription>& OutSockets) const {
	new (OutSockets) FComponentSocketDescription(SocketName, EComponentSocketType::Socket);
}

