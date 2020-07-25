// Fill out your copyright notice in the Description page of Project Settings.


#include "LE_HUD.h"
#include "UI/IndicatorsUI.h"
#include "LE_GameInstance.h"

void ALE_HUD::BeginPlay() {
	Super::BeginPlay();

	Init<UIndicatorsUI>(IndicatorsUIClass, IndicatorsUI, ULE_GameInstance::GetInstance());
}

void ALE_HUD::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	
	UnInit<UIndicatorsUI>(IndicatorsUI);

	Super::EndPlay(EndPlayReason);
}