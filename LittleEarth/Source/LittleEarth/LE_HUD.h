// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/IndicatorsUI.h"
#include "LE_HUD.generated.h"

class UIndicatorsUI;
class UMenuUI;
class UGameInstance;

UCLASS()
class LITTLEEARTH_API ALE_HUD : public AHUD {

	GENERATED_BODY()

public:

	template<typename T>
	void Init(TSubclassOf<T> itemClass, T* itemPtr, UGameInstance* gameInstance) {
		
		if (itemClass != nullptr) {

			itemPtr = CreateWidget<T, UGameInstance>(gameInstance, itemClass);
			checkSlow(itemPtr != nullptr);
			itemPtr->AddToViewport();
			itemPtr->Subscribe();
		}
	}

	template<typename T>
	void UnInit(T* itemPtr) {

		if (itemPtr != nullptr) {
			itemPtr->Unsubscribe();
		}
	}

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UIndicatorsUI> IndicatorsUIClass;

	UIndicatorsUI* IndicatorsUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UMenuUI> MenuUIClass;

	UMenuUI* MenuUI;

};
