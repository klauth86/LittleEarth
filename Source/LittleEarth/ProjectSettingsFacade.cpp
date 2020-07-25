#include "ProjectSettingsFacade.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "Utils/LogManager.h"

FVector UProjectSettingsFacade::GetGravity() {
	static FVector gravity = FVector(0, 0, GetDefaultGravityZ());
	return gravity;
}

FVector UProjectSettingsFacade::GetGravityDir() {
	static FVector gravityDir = FVector(0, 0, -1);
	return gravityDir;
}

FVector UProjectSettingsFacade::GetGravity(FVector location, FVector center) {
	return GetGravityDir(location, center)* GetDefaultGravityZ();
}

FVector UProjectSettingsFacade::GetGravityDir(FVector location, FVector center) {
	//LogManager::LogWarning(TEXT("C[%s] - L[%s] = [%s]"), *center.ToString(), *location.ToString(), *(center - location).ToString());
	return (center - location).GetSafeNormal();
}

float UProjectSettingsFacade::GetDefaultGravityZ() {
	static float defaultGravityZ = -UPhysicsSettings::Get()->DefaultGravityZ;
	return defaultGravityZ;
}