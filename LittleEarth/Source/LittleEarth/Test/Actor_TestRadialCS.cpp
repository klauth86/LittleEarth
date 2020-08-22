// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_TestRadialCS.h"
#include "DrawDebugHelpers.h"

// Sets default values
AActor_TestRadialCS::AActor_TestRadialCS()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AActor_TestRadialCS::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActor_TestRadialCS::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	auto i_num = 16;
	auto j_num = 16;

	for (auto i = 1; i <= i_num; i++) {

		auto gamma = PI * i / i_num;

		for (auto j = 1; j <= j_num; j++) {

			auto betta = 2 * PI * j / j_num;

			auto cosGamma = FMath::Cos(gamma);
			auto sinGamma = FMath::Sin(gamma);

			auto cosBetta = FMath::Cos(betta);
			auto sinBetta = FMath::Sin(betta);

			auto start = GetActorLocation() + 100 * FVector(sinBetta, cosBetta * sinGamma, cosBetta * cosGamma);

			FVector Offset = FVector(150, 0, 0);

			auto end = start + FVector(
				sinBetta * Offset.X + cosBetta * Offset.Y,
				cosBetta * sinGamma * Offset.X - sinBetta * sinGamma * Offset.Y - cosGamma * Offset.Z,
				cosBetta * cosGamma * Offset.X - sinBetta * cosGamma * Offset.Y - sinGamma * Offset.Z
			);

			DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 0, 0, 2.f);
			DrawDebugSphere(GetWorld(), end, 4, 8, FColor::Blue, false, 0, 0, 2.f);

		}
	}
}

