#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/ActorComponent.h"
#include "OwnerMulticasterComponentBase.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOwnerMulticasterComponentBase : public UInterface {
	GENERATED_BODY()
};

class IOwnerMulticasterComponentBase {
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual UActorComponent* GetSelfAsActorComponent() { return nullptr; };

	template<typename T>
	T* GetOwnerAs() {

		if (auto component = GetSelfAsActorComponent()) {
			auto owner = component->GetOwner();
			auto casted = Cast<T>(owner);
			return casted;
		}

		return nullptr;
	}
};
