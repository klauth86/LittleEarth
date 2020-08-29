#pragma once

#include "TimerManager.h"
#include "Utils/LogManager.h"
#include "LE_Common.h"

struct FGameTask_Timer {

	float Time;

	float TimeLeft;

	int8 NumToExecute;

	int8 NumToExecuteLeft;

	FTimerUnifiedDelegate Delegate;

	bool IsInfinit() { return NumToExecute == LE_Common::EXECUTE_INFINIT_TIMES; }

	void Advance(float DeltaSeconds) {		
		TimeLeft -= DeltaSeconds;
		if (TimeLeft <= 0) {
			if (!IsInfinit()) // DECREMENT IF NOT INFINIT
				NumToExecuteLeft--;
		}
	};

	void PostAdvance() {
		if (NumToExecuteLeft > 0 || IsInfinit()) {
			TimeLeft = Time;
		}
	}
};