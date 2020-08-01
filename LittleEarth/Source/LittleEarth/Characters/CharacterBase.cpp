// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "Engine/CollisionProfile.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PawnMovementComponent_Base.h"

#include "Utils/LogManager.h"
#include "Utils/EnumHelper.h"

TArray<ACharacterBase*> ACharacterBase::AllInstances;

ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	struct FConstructorStatics {
		FName ID_Characters;
		FText NAME_Characters;
		FConstructorStatics()
			: ID_Characters(TEXT("Characters"))
			, NAME_Characters(NSLOCTEXT("SpriteCategory", "Characters", "Characters")) {}
	};
	static FConstructorStatics ConstructorStatics;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	RootComponent = MeshComponent = CreateOptionalDefaultSubobject<USkeletalMeshComponent>("Mesh");
	if (MeshComponent) {
		MeshComponent->BodyInstance.SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
		MeshComponent->SetSimulatePhysics(true);
		MeshComponent->SetAngularDamping(0.1f);
		MeshComponent->SetLinearDamping(0.1f);
		MeshComponent->BodyInstance.MassScale = 756;
		MeshComponent->BodyInstance.MaxAngularVelocity = 800.0f;
		MeshComponent->SetNotifyRigidBodyCollision(true);
	}
}

void ACharacterBase::BeginPlay() {
	Super::BeginPlay();
	AllInstances.Add(this);
}

void ACharacterBase::EndPlay(EEndPlayReason::Type EndPlayReason) {
	AllInstances.Remove(this);
	Super::EndPlay(EndPlayReason);
}

FVector ACharacterBase::ConsumeMovementInputVector() {
	return Internal_ConsumeMovementInputVector();
}

void ACharacterBase::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce) {
	Internal_AddMovementInput(WorldDirection * ScaleValue, bForce);
}