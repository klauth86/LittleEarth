// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "PlayerBase.generated.h"

UCLASS()
class APlayerBase : public ACharacterBase {

	GENERATED_BODY()

public:

	APlayerBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MovementEngine)
		float MovementPower;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class ULE_RadialArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

public:

	FORCEINLINE class ULE_RadialArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void BeginPlay() override;

	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void Tick(float DeltaSeconds) override;

protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void StartJumpInput(bool primaryInput);
	
	UFUNCTION()
	void EndJumpInput(bool primaryInput);

	void MoveForward(float Value);

	void MoveRight(float Value);

	void ProcessMovementInput();

	bool TurnToDirection(float turnRatio);

	void MoveToDirection(float moveRatio);

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
