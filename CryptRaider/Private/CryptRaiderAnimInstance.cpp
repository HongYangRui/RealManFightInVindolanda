// Fill out your copyright notice in the Description page of Project Settings.


#include "CryptRaiderAnimInstance.h"
#include "CryptRaider/CryptRaiderCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UCryptRaiderAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	CryptRaiderCharacter = Cast<ACryptRaiderCharacter>(TryGetPawnOwner());
	CryptRaiderCharacterMovement = CryptRaiderCharacter ? CryptRaiderCharacter->GetCharacterMovement() : nullptr;
}
void UCryptRaiderAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (CryptRaiderCharacterMovement) {
		GroundSpeed = UKismetMathLibrary::VSizeXY(CryptRaiderCharacterMovement->Velocity);
		IsFalling = CryptRaiderCharacterMovement->IsFalling();
		CharacterState = CryptRaiderCharacter->GetCharacterState();
		IsDancing = CryptRaiderCharacter->bIsDancing;
		IsCrouching = CryptRaiderCharacter->bIsCrouching;
		ActionState = CryptRaiderCharacter->GetActionState();
		DeathPose = CryptRaiderCharacter->GetDeathPose();
		IsDead = (ActionState == EActionState::EAS_Dead);
	}
	
}