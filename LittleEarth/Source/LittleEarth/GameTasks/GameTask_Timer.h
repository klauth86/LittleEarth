#pragma once

#include "TimerManager.h"
#include "Utils/LogManager.h"

struct FGameTask_Timer {

	float TimeLeft;

	FTimerUnifiedDelegate Delegate;

	void Advance(float DeltaSeconds) {
		TimeLeft -= DeltaSeconds;
		LogManager::LogWarning(TEXT("________ %f"), TimeLeft);
	};
};