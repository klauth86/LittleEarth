// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "LE_RadialArmComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LITTLEEARTH_API ULE_RadialArmComponent : public USceneComponent {

	GENERATED_BODY()

public:
	ULE_RadialArmComponent();

	virtual void OnRegister() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	static const FName SocketName;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radial Arm")
		float HeightOffset = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radial Arm")
		float BettaAngleInDegrees = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radial Arm")
		uint32 bCalculateBettaAngle : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radial Arm")
		float BettaAngleArcLength = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radial Arm")
		uint32 bEnableCameraLag : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radial Arm", meta = (editcondition = "bEnableCameraLag", ClampMin = "0.0", ClampMax = "1000.0", UIMin = "0.0", UIMax = "1000.0"))
		float CameraLagSpeed;

	FVector SocketLocation = FVector::ZeroVector;
	FQuat SocketRotation;

	// USceneComponent interface
	virtual bool HasAnySockets() const override;
	virtual FTransform GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace = RTS_World) const override;
	virtual void QuerySupportedSockets(TArray<FComponentSocketDescription>& OutSockets) const override;
	// End of USceneComponent interface

	void UpdateSocketTransform(bool enableCameraLag, float DeltaTime);
};
