// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "Containers/Queue.h"
#include "PlayerBase.generated.h"

class ULE_RadialArmComponent;
class UCameraComponent;
class UPointLightComponent;
class UCapsuleComponent;

UCLASS()
class APlayerBase : public ACharacterBase {

	GENERATED_BODY()

public:

	APlayerBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MovementEngine)
		float MovementPower;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MovementEngine)
		float JetPackPower;

	static const FName HeadlightSocketName;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		ULE_RadialArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UPointLightComponent* Headlight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCapsuleComponent* CapsuleComponent;

	uint8 IsBraking : 1;

	uint8 JetPackCycles_Max = 8;

	uint8 JetPackCycles_Current = 0;

	TQueue<uint8> JetPackCycles;

public:

	FORCEINLINE ULE_RadialArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE UPointLightComponent* GetHeadlight() const { return Headlight; }

	FORCEINLINE UCapsuleComponent* GetCapsule() const { return CapsuleComponent; }

	void BeginPlay() override;

	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void Tick(float DeltaSeconds) override;

protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void StartJumpInput();

	void StartBraking() { AddOrRemoveBrakingInertia(IsBraking = true); }

	void EndBraking() { AddOrRemoveBrakingInertia(IsBraking = false); }

	void MoveForward(float Value);

	void MoveRight(float Value);

	void ToggleHeadlight();

	void ProcessMovementInput();

	void ProcessJumpInput();

	bool TurnToDirection(float turnRatio);

	void MoveToDirection(float moveRatio);

	void Brake();

	void AddOrRemoveBrakingInertia(bool add);

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
