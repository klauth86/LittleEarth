#pragma once

#include "CoreMinimal.h"
#include "GameTask_Timer.h"

class GameTaskService {

protected:
	
	static FCriticalSection Locker;

	static TMap<FGuid, FGameTask_Timer> Tasks;

public:

	static void Reset() {
		FScopeLock Lock(&Locker);

		Tasks.Empty();
	}

	static void RemoveGameTask(FGuid id) {
		FScopeLock Lock(&Locker);

		if (Tasks.Contains(id)) {
			Tasks.Remove(id);
		}
	}

	static void Advance(float DeltaSeconds) {
		FScopeLock Lock(&Locker);

		for (TMap<FGuid, FGameTask_Timer>::TIterator It(Tasks); It; ++It) {
			auto& task = It->Value;

			task.Advance(DeltaSeconds);
			if (task.TimeLeft <= 0)
				task.Delegate.Execute();

			task.PostAdvance();
			if (task.NumToExecute <= 0)
				It.RemoveCurrent();
		}
	}

	static bool GetGameTask(FGuid id, float& timeLeft) {
		FScopeLock Lock(&Locker);

		if (Tasks.Contains(id)) {
			timeLeft = Tasks[id].TimeLeft;
			return true;
		}

		return false;
	}

	template< class UserClass >
	FORCEINLINE static FGuid AddGameTask(float DeltaSeconds, UserClass* InObj, typename FTimerDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr InTimerMethod, uint8 numToExecute = 1) {
		return AddGameTask_Internal(DeltaSeconds, FTimerUnifiedDelegate(FTimerDelegate::CreateUObject(InObj, InTimerMethod)), numToExecute);
	}
	template< class UserClass >
	FORCEINLINE static FGuid AddGameTask(float DeltaSeconds, UserClass* InObj, typename FTimerDelegate::TUObjectMethodDelegate_Const< UserClass >::FMethodPtr InTimerMethod, uint8 numToExecute = 1) {
		return AddGameTask_Internal(DeltaSeconds, FTimerUnifiedDelegate(FTimerDelegate::CreateUObject(InObj, InTimerMethod)), numToExecute);
	}

	/** Version that takes any generic delegate. */
	FORCEINLINE static FGuid AddGameTask(float DeltaSeconds, FTimerDelegate const& InDelegate, uint8 numToExecute = 1) {
		return AddGameTask_Internal(DeltaSeconds, FTimerUnifiedDelegate(InDelegate), numToExecute);
	}
	/** Version that takes a dynamic delegate (e.g. for UFunctions). */
	FORCEINLINE static FGuid AddGameTask(float DeltaSeconds, FTimerDynamicDelegate const& InDynDelegate, uint8 numToExecute = 1) {
		return AddGameTask_Internal(DeltaSeconds, FTimerUnifiedDelegate(InDynDelegate), numToExecute);
	}
	/** Version that takes a TFunction */
	FORCEINLINE static FGuid AddGameTask(float DeltaSeconds, TFunction<void(void)>&& Callback, uint8 numToExecute = 1) {
		return AddGameTask_Internal(DeltaSeconds, FTimerUnifiedDelegate(MoveTemp(Callback)), numToExecute);
	}

protected:

	static FGuid AddGameTask_Internal(float inDeltaSeconds, FTimerUnifiedDelegate&& InDelegate, uint8 numToExecute) {
		FScopeLock Lock(&Locker);

		auto key = FGuid::NewGuid();

		Tasks.Emplace(key);
		auto& task = Tasks[key];
		task.Time = inDeltaSeconds;
		task.TimeLeft = inDeltaSeconds;
		task.NumToExecute = numToExecute;
		task.NumToExecuteLeft = numToExecute;
		task.Delegate = MoveTemp(InDelegate);

		return key;
	}
};
