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

		auto size2D = FMath::Sqrt(ownerLocation.Z * ownerLocation.Z + ownerLocation.Y * ownerLocation.Y);
		auto size = ownerLocation.Size();

		auto cosGamma = size2D ? ownerLocation.Z / size2D : 1;
		auto sinGamma = size2D ? ownerLocation.Y / size2D : 0;

		auto delta = bCalculateBettaAngle ? (BettaAngleArcLength / size) : BettaAngleInDegrees * PI / 180;
		auto cosDelta = FMath::Cos(delta);
		auto sinDelta = FMath::Sin(delta);

		// From coordinates equations
		//	x = r * sin(b)
		//	y = r * cos(b) * sin(g)
		//	z = r * cos(b) * cos(g)
		auto size_sinGamma = size * sinGamma;

		auto sinBetta = size_sinGamma ? ownerLocation.X / size : 0;
		auto cosBetta = size_sinGamma ? ownerLocation.Y / size_sinGamma : 1;

		auto cosCamera = cosBetta * cosDelta - sinBetta * sinDelta;
		auto sinCamera = sinBetta * cosDelta - sinDelta * cosBetta;

		auto height = size + HeightOffset;

		FVector DesiredSocketLoc = FVector(
				sinCamera *height,
				cosBetta * sinGamma * height,
				cosBetta * cosGamma * height
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

