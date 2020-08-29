// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnProfile_Transform_Sphere.h"

FTransform USpawnProfile_Transform_Sphere::GetSpawnTransform() {
	FTransform result;

	auto radius = FMath::RandRange(MinRadius, MaxRadius);

	auto phi = FMath::RandRange(MinPhi, MaxPhi);
	auto cosPhi = FMath::Cos(phi);
	auto sinPhi = FMath::Sin(phi);

	auto theta = FMath::RandRange(MinTheta, MaxTheta);
	auto cosTheta = FMath::Cos(theta);
	auto sinTheta = FMath::Sin(theta);

	FVector location;
	location.X = radius * cosPhi * sinTheta;
	location.Y = radius * sinPhi * sinTheta;
	location.Z = radius * cosTheta;

	result.SetLocation(location);

	return result;
}