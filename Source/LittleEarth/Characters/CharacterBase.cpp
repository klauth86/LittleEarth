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

	RootComponent = CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
	CapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
	CapsuleComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	CapsuleComponent->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponent->SetCanEverAffectNavigation(false);
	CapsuleComponent->bDynamicObstacle = true;

	CharacterMovementComponent = CreateDefaultSubobject<UPawnMovementComponent_Base>("CharacterMovement");
	CharacterMovementComponent->SetUpdatedComponent(CapsuleComponent);

	Mesh = CreateOptionalDefaultSubobject<USkeletalMeshComponent>("Mesh");
	if (Mesh) {
		Mesh->AlwaysLoadOnClient = true;
		Mesh->AlwaysLoadOnServer = true;
		Mesh->bOwnerNoSee = false;
		Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
		Mesh->bCastDynamicShadow = true;
		Mesh->bAffectDynamicIndirectLighting = true;
		Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		Mesh->SetupAttachment(CapsuleComponent);

		Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
		Mesh->SetGenerateOverlapEvents(false);
		Mesh->SetCanEverAffectNavigation(false);
	}
}

void ACharacterBase::BeginPlay() {
	Super::BeginPlay();
	AllInstances.Add(this);

	FVector vector;
	vector = FVector(100, 0, 0);
	LogManager::LogWarning(TEXT("__ LOC[%s] ROT[%s] OROT[%s]"), *vector.ToString(), *vector.Rotation().ToString(), *vector.ToOrientationQuat().ToString());

	vector = FVector(0, 100, 0);
	LogManager::LogWarning(TEXT("__ LOC[%s] ROT[%s] OROT[%s]"), *vector.ToString(), *vector.Rotation().ToString(), *vector.ToOrientationQuat().ToString());

	vector = FVector(0, 0, 100);
	LogManager::LogWarning(TEXT("__ LOC[%s] ROT[%s] OROT[%s]"), *vector.ToString(), *vector.Rotation().ToString(), *vector.ToOrientationQuat().ToString());
}

void ACharacterBase::EndPlay(EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	AllInstances.Remove(this);
}

FVector ACharacterBase::ConsumeMovementInputVector() {
	return Internal_ConsumeMovementInputVector();
}

void ACharacterBase::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce) {
	Internal_AddMovementInput(WorldDirection * ScaleValue, bForce);
}

void ACharacterBase::FillContext(FMovementContext_Eval& context) {
	context.InputVector = ConsumeMovementInputVector();
	context.RotationUpdateMode = ERotationUpdateMode::PRIMARY;
}