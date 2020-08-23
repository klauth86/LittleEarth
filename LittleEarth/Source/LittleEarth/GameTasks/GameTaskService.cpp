#include "GameTaskService.h"

FCriticalSection GameTaskService::Locker;

TMap<FGuid, FGameTask_Timer> GameTaskService::Tasks;