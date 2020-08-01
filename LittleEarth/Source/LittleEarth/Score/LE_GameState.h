#pragma once

#include "ObjectMacros.h"
#include "LE_GameState.generated.h"

USTRUCT(BlueprintType)
struct FLE_GameState {

	GENERATED_USTRUCT_BODY()

		FLE_GameState() { Reset(); }

	FLE_GameState(const FLE_GameState& other) { Reset(other); }

	UPROPERTY(EditAnywhere, Category = "Game State", BlueprintReadWrite)
		int StarsToCollect;

	UPROPERTY(EditAnywhere, Category = "Game State", BlueprintReadWrite)
		float TimeToSucceed;

	UPROPERTY(EditAnywhere, Category = "Game State", BlueprintReadWrite)
		float TimeToCollapse;

	void Reset() {
		StarsToCollect = 0;
		TimeToSucceed = 0;
		TimeToCollapse = 0;
	}

	void Reset(const FLE_GameState& other) {
		StarsToCollect = other.StarsToCollect;
		TimeToSucceed = other.TimeToSucceed;
		TimeToCollapse = other.TimeToCollapse;
	}
};