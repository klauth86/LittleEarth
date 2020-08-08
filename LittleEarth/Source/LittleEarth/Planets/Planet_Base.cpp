// Fill out your copyright notice in the Description page of Project Settings.


#include "Planet_Base.h"
#include "LE_GameInstance.h"

APlanet_Base::APlanet_Base(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {
	PrimaryActorTick.bCanEverTick = true;
}