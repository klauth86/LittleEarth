// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/LE_SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"

#include "Pickups/Pickup_Base.h"

#include "LE_HUD.h"

#include "EngineUtils.h"
#include "Utils/InputBindingsHelper.h"
#include "Utils/LogManager.h"

APlayerBase::APlayerBase(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {
	
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	MovementPower = 1000;
	TurnArm = 1;

	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<ULE_SpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, ULE_SpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
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

void APlayerBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerBase::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerBase::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerBase::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerBase::LookUpAtRate);
}

void APlayerBase::MoveForward(float Value) {
	if ((Controller != NULL) && (Value != 0.0f)) {
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerBase::MoveRight(float Value) {
	if ((Controller != NULL) && (Value != 0.0f)) {
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void APlayerBase::ProcessMovementInput() {

	MeshComponent->AddForce(10000 * ConsumeMovementInputVector() * MeshComponent->GetMass());

	//auto input = ConsumeMovementInputVector();
	//auto up = GetActorUpVector();
	//auto horInput = input - up * (input | up);

	//auto direction = horInput.GetSafeNormal();
	//if (direction != FVector::ZeroVector) {
	//	
	//	if (TurnToDirection(direction, up)) {

	//		MeshComponent->AddForce(MovementPower * direction / 6, FName("Wheel_F_R"));
	//		MeshComponent->AddForce(MovementPower * direction / 6, FName("Wheel_M_R"));
	//		MeshComponent->AddForce(MovementPower * direction / 6, FName("Wheel_B_R"));

	//		MeshComponent->AddForce(MovementPower * direction / 6, FName("Wheel_F_L"));
	//		MeshComponent->AddForce(MovementPower * direction / 6, FName("Wheel_M_L"));
	//		MeshComponent->AddForce(MovementPower * direction / 6, FName("Wheel_B_L"));

	//	}

	//}
}

void APlayerBase::TurnAtRate(float Rate) {
	//AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerBase::LookUpAtRate(float Rate) {
	//AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool APlayerBase::TurnToDirection(FVector direction, FVector up) {
	auto forward = GetActorForwardVector();
	auto rotationVector = FVector::CrossProduct(forward, direction);
	auto ratio = (rotationVector | up);

	if (ratio < THRESH_SPLIT_POLY_PRECISELY)
		return true;

	MeshComponent->AddForce(ratio * MovementPower * forward / 6, FName("Wheel_F_R"));
	MeshComponent->AddForce(ratio * MovementPower * forward / 6, FName("Wheel_M_R"));
	MeshComponent->AddForce(ratio * MovementPower * forward / 6, FName("Wheel_B_R"));

	MeshComponent->AddForce(-ratio * MovementPower * forward / 6, FName("Wheel_F_L"));
	MeshComponent->AddForce(-ratio * MovementPower * forward / 6, FName("Wheel_M_L"));
	MeshComponent->AddForce(-ratio * MovementPower * forward / 6, FName("Wheel_B_L"));

	return false;
}

static ALE_HUD* hud = nullptr;

void APlayerBase::BeginPlay() {
	Super::BeginPlay();

	auto controller = GetController<APlayerController>();
	hud = Cast<ALE_HUD>(controller->GetHUD());
	if (hud) {
		hud->Init();
	}
}

void APlayerBase::EndPlay(EEndPlayReason::Type EndPlayReason) {	
	if (hud) {
		hud->UnInit();
	}

	Super::EndPlay(EndPlayReason);
}

void APlayerBase::NotifyActorBeginOverlap(AActor* OtherActor) {
	Super::NotifyActorBeginOverlap(OtherActor);

	if (auto pickUp = Cast<APickup_Base>(OtherActor)) {
		pickUp->PickUp();
	}
}