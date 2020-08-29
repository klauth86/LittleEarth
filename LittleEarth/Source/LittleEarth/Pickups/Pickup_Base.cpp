// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup_Base.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

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