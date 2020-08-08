#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/PhysActor.h"
#include "CharacterBase.generated.h"

class USkeletalMeshComponent;

UCLASS(Blueprintable)
class ACharacterBase : public APawn, public IPhysActor {

	GENERATED_BODY()

public:

	ACharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	UPROPERTY(Category = CharacterBase, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* MeshComponent;

public:

	void BeginPlay() override;

	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual UPrimitiveComponent* GetPrimitiveComponent() const override { return MeshComponent; }

public:

	virtual FVector ConsumeMovementInputVector() override;

	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.0f, bool bForce = false) override;
};