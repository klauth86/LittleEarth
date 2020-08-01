#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CharacterBase.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UPawnMovementComponent_Base;

UCLASS(Blueprintable)
class ACharacterBase : public APawn {

	GENERATED_BODY()

public:

	ACharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	UPROPERTY(Category = CharacterBase, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* MeshComponent;

public:

	FORCEINLINE USkeletalMeshComponent* GetMeshComponent() const { return MeshComponent; }

public:

	void BeginPlay() override;

	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:

	virtual FVector ConsumeMovementInputVector() override;

	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.0f, bool bForce = false) override;

public:

	static TArray<ACharacterBase*> AllInstances;
};