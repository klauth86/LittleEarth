// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/LE_RadialArmComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"

#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"

#include "Pickups/Pickup_Base.h"

#include "LE_HUD.h"
#include "LE_Common.h"

#include "EngineUtils.h"
#include "Utils/InputBindingsHelper.h"
#include "Utils/LogManager.h"

const FName APlayerBase::HeadlightSocketName(TEXT("HeadlightSocket"));

APlayerBase::APlayerBase(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {

	MovementPower = 1;
	JetPackPower = 2;

	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<ULE_RadialArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, ULE_RadialArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	Headlight = CreateDefaultSubobject<UPointLightComponent>("Headlight");
	Headlight->SetupAttachment(MeshComponent, APlayerBase::HeadlightSocketName);
	Headlight->SetVisibility(false);

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
	CapsuleComponent->SetupAttachment(MeshComponent);
	CapsuleComponent->SetCapsuleRadius(100);
	CapsuleComponent->SetCapsuleHalfHeight(100);
	CapsuleComponent->SetCollisionProfileName(LE_Common::OverlapAll_ProfileName);
	CapsuleComponent->SetCanEverAffectNavigation(false);
}

static ALE_HUD* hud = nullptr;

void APlayerBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerBase::StartJumpInput);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerBase::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerBase::MoveRight);

	PlayerInputComponent->BindAction("Brake", IE_Pressed, this, &APlayerBase::StartBraking);
	PlayerInputComponent->BindAction("Brake", IE_Released, this, &APlayerBase::EndBraking);

	PlayerInputComponent->BindAction("Headlight", IE_Pressed, this, &APlayerBase::ToggleHeadlight);

	if (auto controller = Cast<APlayerController>(Controller)) {
		hud = Cast<ALE_HUD>(controller->GetHUD());
		if (hud) { hud->Init(); }
	}
}

void APlayerBase::BeginPlay() {
	Super::BeginPlay();
}

void APlayerBase::EndPlay(EEndPlayReason::Type EndPlayReason) {
	if (hud) { hud->UnInit(); }
	Super::EndPlay(EndPlayReason);
}

void APlayerBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	ProcessMovementInput();
	ProcessJumpInput();
}

void APlayerBase::StartJumpInput() {
	if (JetPackCycles_Current < JetPackCycles_Max) {
		JetPackCycles.Enqueue(1);
		JetPackCycles_Current++;
	}
}

void APlayerBase::MoveForward(float Value) {
	if (CameraBoom && (Value != 0.0f)) {
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void APlayerBase::MoveRight(float Value) {
	if (CameraBoom && (Value != 0.0f)) {
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void APlayerBase::ToggleHeadlight() {
	if (Headlight) { Headlight->ToggleVisibility(); }
}

const FName mainBody("Body");
const auto rightWheels = TArray<FName>({ FName("Wheel_F_R") , FName("Wheel_M_R") , FName("Wheel_B_R") });
const auto leftWheels = TArray<FName>({ FName("Wheel_F_L") , FName("Wheel_M_L") , FName("Wheel_B_L") });
const int angularVelocityMultiplier = 4;

void APlayerBase::ProcessMovementInput() {
	auto input = ConsumeMovementInputVector();
	auto up = GetActorUpVector();
	auto horInput = input - up * (input | up);

	auto direction = horInput.GetSafeNormal();

	if (IsBraking) {
		Brake();
		direction /= angularVelocityMultiplier;
	}



	if (direction != FVector::ZeroVector) {

		auto forward = GetActorForwardVector();
		auto rotationVector = FVector::CrossProduct(forward, direction);

		auto turnRatio = (rotationVector | up);
		auto moveRatio = (forward | direction);

		TurnToDirection(turnRatio);
		MoveToDirection(moveRatio);
	}
}

void APlayerBase::ProcessJumpInput() {
	uint8 cycleIndex;
	while (JetPackCycles.Dequeue(cycleIndex)) {
		JetPackCycles_Current--;

		auto body = MeshComponent->GetBodyInstance(mainBody);

		auto mass = body->GetBodyMass();
		auto weight = mass * LE_Common::EARTH_G;

		body->AddForce(GetActorUpVector() * weight * JetPackPower);
	}
}

bool APlayerBase::TurnToDirection(float turnRatio) {

	auto ratioAbs = FMath::Abs(turnRatio);
	if (ratioAbs < THRESH_SPLIT_POLY_PRECISELY)
		return true;

	if (ratioAbs < 0.25f) // Min rotation power
		turnRatio = 0.25f * FMath::Sign(turnRatio);

	auto angularImpulseDirPerWheel = MovementPower * turnRatio * GetActorRightVector();

	for (auto body : MeshComponent->Bodies) {
		auto mass = body->GetBodyMass();
		auto weight = mass * LE_Common::EARTH_G;

		if (rightWheels.Contains(body->BodySetup->BoneName)) {
			body->AddAngularImpulseInRadians(angularImpulseDirPerWheel * weight, false);
		}
		else if (leftWheels.Contains(body->BodySetup->BoneName)) {
			body->AddAngularImpulseInRadians(-angularImpulseDirPerWheel * weight, false);
		}
	}

	return false;
}

void APlayerBase::MoveToDirection(float moveRatio) {

	auto ratioAbs = FMath::Abs(moveRatio);
	if (ratioAbs < THRESH_SPLIT_POLY_PRECISELY)
		return;

	auto angularImpulseDirPerWheel = MovementPower * moveRatio * GetActorRightVector();

	for (auto body : MeshComponent->Bodies) {
		auto mass = body->GetBodyMass();
		auto weight = mass * LE_Common::EARTH_G;

		if (rightWheels.Contains(body->BodySetup->BoneName)) {
			body->AddAngularImpulseInRadians(angularImpulseDirPerWheel * weight, false);
		}
		else if (leftWheels.Contains(body->BodySetup->BoneName)) {
			body->AddAngularImpulseInRadians(angularImpulseDirPerWheel * weight, false);
		}
	}
}

void APlayerBase::Brake() {

	for (auto body : MeshComponent->Bodies) {

		if (rightWheels.Contains(body->BodySetup->BoneName) || leftWheels.Contains(body->BodySetup->BoneName)) {
			auto oldVelocity = body->GetUnrealWorldAngularVelocityInRadians();
			body->SetAngularVelocityInRadians(oldVelocity / angularVelocityMultiplier, false);
		}
	}
}

void APlayerBase::AddOrRemoveBrakingInertia(bool add) {
	if (add) Brake();
}

void APlayerBase::NotifyActorBeginOverlap(AActor* OtherActor) {
	Super::NotifyActorBeginOverlap(OtherActor);

	if (auto pickUp = Cast<APickup_Base>(OtherActor)) {
		pickUp->PickUp();
	}
}