// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidget_Base.generated.h"

UCLASS()
class LITTLEEARTH_API UUserWidget_Base : public UUserWidget {
	
	GENERATED_BODY()
	
public:

	virtual void Subscribe() {}

	virtual void Unsubscribe() {}

};
