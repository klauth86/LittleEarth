// Fill out your copyright notice in the Description page of Project Settings.


#include "LE_HUD.h"
#include "UI/IndicatorsUI.h"
#include "UI/MenuUI.h"
#include "LE_GameInstance.h"

void ALE_HUD::Init() {

	Init<UIndicatorsUI>(IndicatorsUIClass, IndicatorsUI, ULE_GameInstance::GetInstance());

	Init<UMenuUI>(MenuUIClass, MenuUI, ULE_GameInstance::GetInstance());
}

void ALE_HUD::UnInit() {
	
	UnInit<UIndicatorsUI>(IndicatorsUI);

	UnInit<UMenuUI>(MenuUI);
}