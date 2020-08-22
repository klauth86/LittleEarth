// Fill out your copyright notice in the Description page of Project Settings.


#include "LE_RadialArmComponent.h"
#include "GameFramework/Actor.h"
#include "Utils/LogManager.h"
#include "DrawDebugHelpers.h"
#include "Math/Matrix.h"

const FName ULE_RadialArmComponent::SocketName(TEXT("RadialEndpoint"));

ULE_RadialArmComponent::ULE_RadialArmComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	bAutoActivate = true;
	bTickInEditor = true;

	CameraLagSpeed = 10.f;

	Offset = FVector(-1200, 0, 800);
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
	
	const FVector PreviousSocketLocation = SocketLocation;

	if (auto owner = GetOwner()) {

		auto ownerLocation = owner->GetActorLocation();
		auto upVector = ownerLocation.GetSafeNormal();
		auto forwardVector = GetOwner()->GetActorForwardVector();

		FVector DesiredSocketLoc = Offset.X * forwardVector + Offset.Z * upVector + ownerLocation;

		if (enableCameraLag) {
			SocketLocation = FMath::VInterpTo(PreviousSocketLocation, DesiredSocketLoc, DeltaTime, CameraLagSpeed);
		}
		else {
			SocketLocation = DesiredSocketLoc;
		}

		SocketRotation = FMatrix(forwardVector, FVector::CrossProduct(upVector, forwardVector), upVector, FVector::ZeroVector).Rotator();
	}
}
