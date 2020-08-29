// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup_Base.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "LE_Common.h"
#include "Engine/CollisionProfile.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

APickup_Base::APickup_Base() {
	SetRootComponent(CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent"));
	CapsuleComponent->SetCapsuleRadius(100);
	CapsuleComponent->SetCapsuleHalfHeight(100);
	CapsuleComponent->SetCollisionProfileName(LE_Common::OverlapAll_ProfileName);
	CapsuleComponent->SetCanEverAffectNavigation(false);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	StaticMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	StaticMeshComponent->SetCanEverAffectNavigation(false);
	StaticMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void APickup_Base::BeginPlay() {
	Super::BeginPlay();

	auto world = GetWorld();

	auto transform = GetActorTransform();

	if (Spawn_VFXClass)
		world->SpawnActor(Spawn_VFXClass, &transform);

	if (Spawn_SFX)
		UGameplayStatics::PlaySoundAtLocation(world, PickUp_SFX, GetActorLocation());
}

void APickup_Base::PickUp() {

	auto world = GetWorld();

	auto transform = GetActorTransform();

	if (PickUp_VFXClass)
		world->SpawnActor(PickUp_VFXClass, &transform);

	if (PickUp_SFX)
		UGameplayStatics::PlaySoundAtLocation(world, PickUp_SFX, GetActorLocation());

	Destroy();
}