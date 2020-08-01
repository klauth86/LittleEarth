// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class LogManager {

public:
	template <typename FmtType, typename... Types>
	static FORCEINLINE void LogMessage(const FmtType& Fmt, Types... Args) {
		UE_LOG(LogTemp, Display, Fmt, Args...);
	}

	template <typename FmtType, typename... Types>
	static FORCEINLINE void LogWarning(const FmtType& Fmt, Types... Args) {
		UE_LOG(LogTemp, Warning, Fmt, Args...);
	}

	template <typename FmtType, typename... Types>
	static FORCEINLINE void LogError(const FmtType& Fmt, Types... Args) {
		UE_LOG(LogTemp, Error, Fmt, Args...);
	}

	template <typename FmtType, typename... Types>
	static FORCEINLINE void LogFatal(const FmtType& Fmt, Types... Args) {
		UE_LOG(LogTemp, Fatal, Fmt, Args...);
	}
};
