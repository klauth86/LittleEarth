#pragma once

#include "TimerManager.h"
#include "Utils/LogManager.h"
#include "LE_Common.h"

struct FGameTask_Timer {

	float Time;

	float TimeLeft;

	uint8 NumToExecute;

	uint8 NumToExecuteLeft;

	FTimerUnifiedDelegate Delegate;

	void Advance(float DeltaSeconds) {		
		TimeLeft -= DeltaSeconds;
		if (TimeLeft <= 0) {

			if (NumToExecute != LE_Common::EXECUTE_INFINIT_TIMES) // DECREMENT IF NOT INFINIT
				NumToExecuteLeft--;
		}
	};

	void PostAdvance() {
		if (NumToExecuteLeft > 0) {
			TimeLeft = Time;
		}
	}
};