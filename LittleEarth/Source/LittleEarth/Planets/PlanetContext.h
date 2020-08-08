#pragma once

#include "ObjectMacros.h"
#include "PlanetContext.generated.h"

USTRUCT(BlueprintType)
struct FPlanetContext {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "Planet Context", BlueprintReadWrite)
		float TimeToCollapse;

};