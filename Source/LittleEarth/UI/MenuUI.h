// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/UserWidget_Base.h"
#include "MenuUI.generated.h"

UCLASS()
class LITTLEEARTH_API UMenuUI : public UUserWidget_Base {

	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent)
		void OnIsOpenChanged();

	UFUNCTION(BlueprintCallable)
		void SetIsOpen(bool isOpen, bool isForced = false) {
		if (IsOpen != isOpen || isForced) {
			IsOpen = isOpen;
			OnIsOpenChanged();
		}
	}


protected:

	UPROPERTY(BlueprintReadOnly)
		bool IsOpen = false;
};
