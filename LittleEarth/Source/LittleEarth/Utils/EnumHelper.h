#pragma once

#include "CoreMinimal.h"

template<typename TEnum>
static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value) {
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
	if (!enumPtr) {
		return FString("Invalid");
	}
	return enumPtr->GetNameByValue((int64)Value).ToString();
}