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

	static void Advance(float DeltaSeconds) {
		FScopeLock Lock(&Locker);

		for (TMap<FGuid, FGameTask_Timer>::TIterator It(Tasks); It; ++It) {
			auto& task = It->Value;
			
			task.Advance(DeltaSeconds);
			if (task.TimeLeft <= 0) {
				task.Delegate.Execute();
				It.RemoveCurrent();
			}
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
	FORCEINLINE static FGuid AddGameTask(float DeltaSeconds, UserClass* InObj, typename FTimerDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr InTimerMethod) {
		return AddGameTask_Internal(DeltaSeconds, FTimerUnifiedDelegate(FTimerDelegate::CreateUObject(InObj, InTimerMethod)));
	}
	template< class UserClass >
	FORCEINLINE static FGuid AddGameTask(float DeltaSeconds, UserClass* InObj, typename FTimerDelegate::TUObjectMethodDelegate_Const< UserClass >::FMethodPtr InTimerMethod) {
		return AddGameTask_Internal(DeltaSeconds, FTimerUnifiedDelegate(FTimerDelegate::CreateUObject(InObj, InTimerMethod)));
	}

	/** Version that takes any generic delegate. */
	FORCEINLINE static FGuid AddGameTask(float DeltaSeconds, FTimerDelegate const& InDelegate) {
		return AddGameTask_Internal(DeltaSeconds, FTimerUnifiedDelegate(InDelegate));
	}
	/** Version that takes a dynamic delegate (e.g. for UFunctions). */
	FORCEINLINE static FGuid AddGameTask(float DeltaSeconds, FTimerDynamicDelegate const& InDynDelegate) {
		return AddGameTask_Internal(DeltaSeconds, FTimerUnifiedDelegate(InDynDelegate));
	}
	/** Version that takes a TFunction */
	FORCEINLINE static FGuid AddGameTask(float DeltaSeconds, TFunction<void(void)>&& Callback) {
		return AddGameTask_Internal(DeltaSeconds, FTimerUnifiedDelegate(MoveTemp(Callback)));
	}

protected:

	static FGuid AddGameTask_Internal(float inDeltaSeconds, FTimerUnifiedDelegate&& InDelegate) {
		FScopeLock Lock(&Locker);

		auto key = FGuid::NewGuid();

		Tasks.Emplace(key);
		auto& task = Tasks[key];
		task.TimeLeft = inDeltaSeconds;
		task.Delegate = MoveTemp(InDelegate);

		return key;
	}
};
