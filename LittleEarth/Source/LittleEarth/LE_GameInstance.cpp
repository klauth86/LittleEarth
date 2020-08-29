// Fill out your copyright notice in the Description page of Project Settings.


#include "LE_GameInstance.h"
#include "Engine/World.h"
#include "GameTasks/GameTaskService.h"
#include "Utils/LogManager.h"

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

	StartPoll(GetWorld());
}

void ULE_GameInstance::StartOpenLevel(const FLE_GameState& startState) {



	StartPoll(GetWorld());
}

void ULE_GameInstance::StartPoll(UWorld* world) {
	world->GetTimerManager().SetTimer(TimerHandle_Poll, this, &ULE_GameInstance::OnPoll, LE_Common::POLL_RATE, true);
}

void ULE_GameInstance::StopPoll(UWorld* world) {
	world->GetTimerManager().ClearTimer(TimerHandle_Poll);
	GameTaskService::Reset();
}

void ULE_GameInstance::OnPoll() {
	GameTaskService::Advance(LE_Common::POLL_RATE);
	LogManager::LogWarning(TEXT("ULE_GameInstance::OnPoll"));
}