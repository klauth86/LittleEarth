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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MovementEngine)
		float MovementPower;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MovementEngine)
		float TurnArm;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class ULE_SpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

public:

	FORCEINLINE class ULE_SpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void BeginPlay() override;

	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void Tick(float DeltaSeconds) override { 
		Super::Tick(DeltaSeconds);
		ProcessMovementInput();
	}

protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void StartJumpInput(bool primaryInput);
	
	UFUNCTION()
	void EndJumpInput(bool primaryInput);

	void MoveForward(float Value);

	void MoveRight(float Value);

	void ProcessMovementInput();

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	bool TurnToDirection(FVector direction, FVector up);

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
