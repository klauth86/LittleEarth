// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerBase.h"
#include "Components/PawnMovementComponent_Base.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Pickups/Pickup_Base.h"

#include "EngineUtils.h"
#include "Utils/InputBindingsHelper.h"

APlayerBase::APlayerBase(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {
	
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovementComponent()->CharacterContext.bOrientRotationToMovement = true;
	GetCharacterMovementComponent()->CharacterContext.RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovementComponent()->CharacterContext.JumpZVelocity = 600;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
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

void APlayerBase::TurnAtRate(float Rate) {
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerBase::LookUpAtRate(float Rate) {
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerBase::NotifyActorBeginOverlap(AActor* OtherActor) {
	Super::NotifyActorBeginOverlap(OtherActor);

	if (auto pickUp = Cast<APickup_Base>(OtherActor)) {
		pickUp->PickUp();
	}
}