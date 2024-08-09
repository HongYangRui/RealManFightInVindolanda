// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include"CharacterTypes.h"
#include "CoreMinimal.h"
#include "Logging/LogMacros.h"
#include"public/Characters/BaseCharacter.h"
#include "Interfaces/PickupInterface.h"
#include "Components/BoxComponent.h"
#include "CryptRaiderCharacter.generated.h"


class UCharacterMovementComponent;
class USpringArmComponent;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class AItem;
class ASoul;
class AHealingPotion;
class ATreasure;
class UAnimMontage;
class UPlayerOverlay;
DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ACryptRaiderCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()


	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwitchViewAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DanceAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RunAction;
	
public:
	ACryptRaiderCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Treasure) override;
	virtual void AddHealingPotion(AHealingPotion* HealingPotion) override;
protected:
	virtual void BeginPlay();

public:
		
	virtual void Jump() override;
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();
	UFUNCTION(BlueprintCallable)
	void SwitchView();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
	bool bIsCrouching;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsRunning;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	float Speed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsJumping;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsDancing;
	void StartJump();
	void StopJump();

	virtual void AttackEnd() override;
	UFUNCTION(BlueprintCallable)
	void KickEnd();
	UFUNCTION(BlueprintCallable)
	void HitReactEnd();
	UFUNCTION(BlueprintCallable)
	void UseHealthPotion();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Turn(float Value);
	void LookUp(float Value);
	void MoveRight(float Value);
	void MoveForward(float Value);
	void StartCrouch();
	void StopCrouch();
	void StartRun();
	void StopRun();
	virtual void Landed(const FHitResult& Hit) override;
	void StartDance();
	void StopDance();
	virtual void Attack() override;
	void Kick();
	void PlayKickMontage();
	void PlayEquipMontage(const FName& SectionName);
	bool CanDisarm();
	bool CanArm();
	
	void Disarm();
	
	void Arm();
	virtual void Die() override;
	bool HasEnoughStamina();
	bool IsOccupied();
	void Dodge();
	virtual void DodgeEnd() override;

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();
	void EquipWeapon(AWeapon* Weapon);
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface
	void EKeyPressed();
public:

	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
private:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* ThirdPersonCameraComponent;
	bool bIsFirstPerson;
	void UpdateWalkSpeed();
	void InitializeMovementState();

	UPROPERTY(VIsibleInstanceOnly)
	AItem* OverlappingItem;
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* KickMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	UBoxComponent* KickBox;

	UFUNCTION()
	void OnKickHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	virtual bool CanAttack() override;
	bool CanKick();

	void InitializePlayerOverlay();
	UPROPERTY()
	UPlayerOverlay* PlayerOverlay;

	bool IsUnoccupied();
	void SetHUDHealth();

	void SetupKickCollision();
	void EnableKickCollision();
	void DisableKickCollision();
public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
};

