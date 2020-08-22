// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/LE_RadialArmComponent.h"
#include "Camera/CameraComponent.h"

#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"

#include "Pickups/Pickup_Base.h"

#include "LE_HUD.h"
#include "LE_Common.h"

#include "EngineUtils.h"
#include "Utils/InputBindingsHelper.h"
#include "Utils/LogManager.h"

APlayerBase::APlayerBase(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {

	MovementPower = 1;

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
}

static ALE_HUD* hud = nullptr;

void APlayerBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerBase::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerBase::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerBase::MoveRight);

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
}

void APlayerBase::StartJumpInput(bool primaryInput) {
	if (primaryInput) {
		//bPrimaryJumpInput = true;
	}
	//StartJump();
}

void APlayerBase::EndJumpInput(bool primaryInput) {
	if (primaryInput) {
		//bPrimaryJumpInput = false;
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

void APlayerBase::ProcessMovementInput() {

	auto input = ConsumeMovementInputVector();
	auto up = GetActorUpVector();
	auto horInput = input - up * (input | up);

	auto direction = horInput.GetSafeNormal();
	if (direction != FVector::ZeroVector) {

		auto forward = GetActorForwardVector();
		auto rotationVector = FVector::CrossProduct(forward, direction);		
		
		auto turnRatio = (rotationVector | up);
		auto moveRatio = (forward | direction);

		TurnToDirection(turnRatio);
		MoveToDirection(moveRatio);
	}
}

const auto rightWheels = TArray<FName>({ FName("Wheel_F_R") , FName("Wheel_M_R") , FName("Wheel_B_R") });
const auto leftWheels = TArray<FName>({ FName("Wheel_F_L") , FName("Wheel_M_L") , FName("Wheel_B_L") });

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
			auto mass = body->GetBodyMass();
			body->AddAngularImpulseInRadians(angularImpulseDirPerWheel * weight, false);
		}
	}
}

void APlayerBase::NotifyActorBeginOverlap(AActor* OtherActor) {
	Super::NotifyActorBeginOverlap(OtherActor);

	if (auto pickUp = Cast<APickup_Base>(OtherActor)) {
		pickUp->PickUp();
	}
}