#pragma once

#include "Math/Vector.h"

namespace UProjectSettingsFacade {

	extern FVector GetGravity();

	extern FVector GetGravityDir();

	extern FVector GetGravity(FVector location, FVector center = FVector::ZeroVector);

	extern FVector GetGravityDir(FVector location, FVector center = FVector::ZeroVector);

	extern float GetDefaultGravityZ();

};