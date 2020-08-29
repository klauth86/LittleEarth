// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/EngineTypes.h"
#include "Score/LE_GameState.h"
#include "LE_GameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCurrentStateChangedEvent);

UCLASS()
class LITTLEEARTH_API ULE_GameInstance : public UGameInstance {

	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "LE Game Instance")
		static ULE_GameInstance* GetInstance();

	virtual void Init() override;

	virtual void BeginDestroy() override;

	virtual void FinishDestroy() override;

	UFUNCTION(BlueprintCallable)
		void EndOpenLevel(const FLE_GameState& startState);

	UFUNCTION(BlueprintCallable)
		void StartOpenLevel(const FLE_GameState& startState);

// Game persistent state
public:

	FCurrentStateChangedEvent OnCurrentStateChanged;

	int AddStars(int stars) {
		CurrentState.StarsToCollect -= stars;
		return CurrentState.StarsToCollect;
	}

	float AddTime(float time) {
		CurrentState.TimeToSucceed += time;

		if (CurrentState.TimeToSucceed <= 0)
			CurrentState.TimeToSucceed = 0;

		return CurrentState.TimeToSucceed;
	}
	
protected:

	UPROPERTY(BlueprintReadOnly)
		FLE_GameState CurrentState;



// Game Tasks Poll
public:

	void StartPoll(UWorld* world);

	void StopPoll(UWorld* world);

	void OnPoll();

protected:

	FTimerHandle TimerHandle_Poll;
};
