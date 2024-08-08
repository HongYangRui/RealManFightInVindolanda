// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include"CharacterTypes.h"
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CryptRaiderAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CRYPTRAIDER_API UCryptRaiderAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;


	UPROPERTY(BlueprintReadOnly)
	class ACryptRaiderCharacter* CryptRaiderCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* CryptRaiderCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsDancing;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsCrouching;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	EActionState ActionState;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	TEnumAsByte<EDeathPose> DeathPose;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsDead;
};
