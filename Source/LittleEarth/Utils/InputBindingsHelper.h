#pragma once

#include "Components/InputComponent.h"

class InputBindingsHelper {

public:
	template<typename T>
	static void BindActionWithParam(FName& actionName, FName& functionToBind, T param, class AActor* owner, UInputComponent* inputComponent, EInputEvent inputEvent = IE_Pressed) {

		FInputActionBinding ActionBindingOneParam(actionName, inputEvent);

		FInputActionHandlerSignature OneParamActionHandler;
		OneParamActionHandler.BindUFunction(owner, functionToBind, param);
		ActionBindingOneParam.ActionDelegate = OneParamActionHandler;

		inputComponent->AddActionBinding(ActionBindingOneParam);
	}
};