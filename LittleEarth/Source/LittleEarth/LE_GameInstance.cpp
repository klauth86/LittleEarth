// Fill out your copyright notice in the Description page of Project Settings.


#include "LE_GameInstance.h"

static ULE_GameInstance* Instance = nullptr;

ULE_GameInstance* ULE_GameInstance::GetInstance() {
	checkf(Instance != nullptr, TEXT("Someone is trying to use the game	instance before it has initialized!"));
	return Instance;
}

void ULE_GameInstance::Init() {
	Super::Init();
	Instance = this;
	AddToRoot();

	CurrentState.Reset();
}

void ULE_GameInstance::BeginDestroy() {

	// TODO Clear Persistent Data

	Super::BeginDestroy();
}

void ULE_GameInstance::FinishDestroy() {
	Super::FinishDestroy();
}

void ULE_GameInstance::EndOpenLevel(const FLE_GameState& startState) {

	CurrentState.Reset(startState);
	OnCurrentStateChanged.Broadcast();

}