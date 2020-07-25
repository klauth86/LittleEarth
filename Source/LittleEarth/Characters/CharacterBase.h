#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Movement/ActorWithMovementContext.h"
#include "CharacterBase.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UPawnMovementComponent_Base;

UCLASS(Blueprintable)
class ACharacterBase : public APawn, public IActorWithMovementContext {

	GENERATED_BODY()

public:

	ACharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	UPROPERTY(Category = CharacterBase, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* Mesh;

	UPROPERTY(Category = CharacterBase, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UCapsuleComponent* CapsuleComponent;

	UPROPERTY(Category = CharacterBase, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UPawnMovementComponent_Base* CharacterMovementComponent;

public:

	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }

	FORCEINLINE UCapsuleComponent* GetCapsuleComponent() const { return CapsuleComponent; }

	FORCEINLINE UPawnMovementComponent_Base* GetCharacterMovementComponent() const { return CharacterMovementComponent; }

public:

	void BeginPlay() override;

	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:

	virtual FVector ConsumeMovementInputVector() override;

	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.0f, bool bForce = false) override;

	virtual void FillContext(FMovementContext_Eval& context);

public:

	static TArray<ACharacterBase*> AllInstances;
};