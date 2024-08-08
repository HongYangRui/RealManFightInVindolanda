// Copyright Epic Games, Inc. All Rights Reserved.

#include "CryptRaiderCharacter.h"
#include "CryptRaiderProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include"GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include"Items/Item.h"
#include"Items/Weapons/Weapon.h"
#include"Animation/AnimMontage.h"
#include "AttributeComponent.h"
#include "HUD/CryptHUD.h"
#include "HUD/PlayerOverlay.h"
#include"CryptRaiderAnimInstance.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Items/HealingPotion.h"
DEFINE_LOG_CATEGORY(LogTemplateCharacter);

// ACryptRaiderCharacter

ACryptRaiderCharacter::ACryptRaiderCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bIsDancing = false;
	bIsJumping = false;
	bIsCrouching = false;
	bIsRunning = false;
	Speed = 0.0f;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	// Character doesnt have a rifle at start
	bHasRifle = false;
	bIsFirstPerson = true; // Start in first person
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	// Create the camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;


	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom
	ThirdPersonCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void ACryptRaiderCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Attributes && PlayerOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		PlayerOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ACryptRaiderCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	Tags.Add(FName("EngageableTarget"));
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	FirstPersonCameraComponent->SetActive(true);
	ThirdPersonCameraComponent->SetActive(false);
	InitializeMovementState();
	InitializePlayerOverlay();
}

void ACryptRaiderCharacter::InitializeMovementState()
{
    bIsRunning = false; // 设置为默认行走状态
    GetCharacterMovement()->MaxWalkSpeed = 300.0f; // 设置默认的行走速度
}

// Input

void ACryptRaiderCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		/*EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);*/
		 /* Crouching*/
		/*EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ACryptRaiderCharacter::StartCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ACryptRaiderCharacter::StopCrouch);*/
		PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ACryptRaiderCharacter::StartCrouch);
		PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ACryptRaiderCharacter::StopCrouch);
		PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ACryptRaiderCharacter::StartRun);
		PlayerInputComponent->BindAction("Run", IE_Released, this, &ACryptRaiderCharacter::StopRun);
		PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACryptRaiderCharacter::StartJump);
		PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACryptRaiderCharacter::StopJumping);
		PlayerInputComponent->BindAction("Dance", IE_Pressed, this, &ACryptRaiderCharacter::StartDance);
		PlayerInputComponent->BindAction("Dance", IE_Released, this, &ACryptRaiderCharacter::StopDance);
		PlayerInputComponent->BindAction("Equip", IE_Released, this, &ACryptRaiderCharacter::EKeyPressed);
		PlayerInputComponent->BindAction("Attack", IE_Released, this, &ACryptRaiderCharacter::Attack);
		PlayerInputComponent->BindAction("Kick", IE_Released, this, &ACryptRaiderCharacter::Kick);
		PlayerInputComponent->BindAction("Dodge", IE_Released, this, &ACryptRaiderCharacter::Dodge);
		PlayerInputComponent->BindAction("UseHealthPotion", IE_Released, this, &ACryptRaiderCharacter::UseHealthPotion);
		//// Moving
		//EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACryptRaiderCharacter::Move);

		//// Looking
		//EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACryptRaiderCharacter::Look);
		// Switching view
		EnhancedInputComponent->BindAction(SwitchViewAction, ETriggerEvent::Triggered, this, &ACryptRaiderCharacter::SwitchView);
		PlayerInputComponent->BindAxis("Turn", this, &ACryptRaiderCharacter::Turn);
		PlayerInputComponent->BindAxis("LookUp", this, &ACryptRaiderCharacter::LookUp);
		PlayerInputComponent->BindAxis("MoveRight", this, &ACryptRaiderCharacter::MoveRight);
		PlayerInputComponent->BindAxis("MoveForward", this, &ACryptRaiderCharacter::MoveForward);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


//void ACryptRaiderCharacter::Move(const FInputActionValue& Value)
//{
//	// input is a Vector2D
//	FVector2D MovementVector = Value.Get<FVector2D>();
//
//	if (Controller != nullptr)
//	{
//		// add movement 
//		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
//		AddMovementInput(GetActorRightVector(), MovementVector.X);
//	}
//}

//void ACryptRaiderCharacter::Look(const FInputActionValue& Value)
//{
//	// input is a Vector2D
//	FVector2D LookAxisVector = Value.Get<FVector2D>();
//
//	if (Controller != nullptr)
//	{
//		// add yaw and pitch input to controller
//		AddControllerYawInput(LookAxisVector.X);
//		AddControllerPitchInput(LookAxisVector.Y);
//	}
//}

void ACryptRaiderCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool ACryptRaiderCharacter::GetHasRifle()
{
	return bHasRifle;
}

void ACryptRaiderCharacter::SwitchView()
{
	bIsFirstPerson = !bIsFirstPerson;

	if (bIsFirstPerson)
	{
		FirstPersonCameraComponent->SetActive(true);
		ThirdPersonCameraComponent->SetActive(false);
		// First person view settings
		GetMesh()->SetOwnerNoSee(true);
		GetMesh()->SetRelativeLocation(FVector(-30.f, 0.f, -150.f)); // Adjust to show only arms
		bUseControllerRotationYaw = true;
		CameraBoom->bUsePawnControlRotation = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		FirstPersonCameraComponent->SetActive(false);
		ThirdPersonCameraComponent->SetActive(true);
		// Third person view settings
		GetMesh()->SetOwnerNoSee(false);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f)); // Reset to full body position
		bUseControllerRotationYaw = false;
		CameraBoom->bUsePawnControlRotation = true;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}


void ACryptRaiderCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ACryptRaiderCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}



void ACryptRaiderCharacter::MoveRight(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ACryptRaiderCharacter::MoveForward(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (Controller != nullptr)
	{
		const FRotator ControlRotation=GetControlRotation();
		const FRotator ControlRotationYaw=FRotator(0,ControlRotation.Yaw,0);
		const FVector Forward=FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::X);
		AddMovementInput(Forward, Value);
	}
}
void ACryptRaiderCharacter::StartCrouch()
{
	Crouch();
	bIsCrouching = true;
	UpdateWalkSpeed();
}

void ACryptRaiderCharacter::StopCrouch()
{
	UnCrouch();
	bIsCrouching = false;
	UpdateWalkSpeed();
}

void ACryptRaiderCharacter::StartRun()
{
	if (!bIsCrouching)
	{
		bIsRunning = true;
		UpdateWalkSpeed();
	}
}

void ACryptRaiderCharacter::StopRun()
{
	bIsRunning = false;
	UpdateWalkSpeed();
}

void ACryptRaiderCharacter::UpdateWalkSpeed()
{
	if (bIsCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = 150.0f; 
	}
	else if (bIsRunning)
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.0f; 
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 300.0f; 
	}
}

void ACryptRaiderCharacter::StartJump()
{
	
	bIsJumping = true;
	Jump();
}

void ACryptRaiderCharacter::StopJump()
{
	
	bIsJumping = false;
	StopJumping();
}

void ACryptRaiderCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	bIsJumping = false;
}

void ACryptRaiderCharacter::StartDance()
{
	bIsDancing = true;
}

void ACryptRaiderCharacter::StopDance()
{
	bIsDancing = false;
}

void ACryptRaiderCharacter::Attack()
{

	Super::Attack();
	if (CanAttack())
	{
		/*UE_LOG(LogTemplateCharacter, Warning, TEXT("Attack"));*/
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
		SetWeaponCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

bool ACryptRaiderCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
	   
}

void ACryptRaiderCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
	//UE_LOG(LogTemplateCharacter, Warning, TEXT("AttackEnd"));
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
}





void ACryptRaiderCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon) {
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		if (CanDisarm())
		{
			Disarm();
		}
		else if (CanArm())
		{
			Arm();
		}
	}
}

void ACryptRaiderCharacter::Kick()
{
	if (CanKick())
	{
		PlayKickMontage();
		ActionState = EActionState::EAS_Kicking;
	}
}

bool ACryptRaiderCharacter::CanKick()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void ACryptRaiderCharacter::InitializePlayerOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		ACryptHUD* CryptHUD = Cast<ACryptHUD>(PlayerController->GetHUD());
		if (CryptHUD)
		{
			PlayerOverlay = CryptHUD->GetPlayerOverlay();
			if (PlayerOverlay && Attributes)
			{
				PlayerOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				PlayerOverlay->SetStaminaBarPercent(1.f);
				PlayerOverlay->SetGold(0);
				PlayerOverlay->SetSouls(0);
				PlayerOverlay->SetHealingPotion(3);
			}
		}
	}
}

bool ACryptRaiderCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void ACryptRaiderCharacter::SetHUDHealth()
{
	if (PlayerOverlay && Attributes)
	{
		PlayerOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void ACryptRaiderCharacter::KickEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ACryptRaiderCharacter::PlayKickMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && KickMontage)
	{
		AnimInstance->Montage_Play(KickMontage);
	}
}

void ACryptRaiderCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

bool ACryptRaiderCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped ;
}

bool ACryptRaiderCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped&&
		EquippedWeapon;
}

void ACryptRaiderCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ACryptRaiderCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedTwoHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ACryptRaiderCharacter::Die()
{
	Super::Die();

	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
	UCryptRaiderAnimInstance* AnimInstance = Cast<UCryptRaiderAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->IsDead = true;
	}
}

bool ACryptRaiderCharacter::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}

bool ACryptRaiderCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}

void ACryptRaiderCharacter::Dodge()
{
	if (IsOccupied() || !HasEnoughStamina()) return;
	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;
	if (Attributes && PlayerOverlay)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
		PlayerOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ACryptRaiderCharacter::DodgeEnd()
{
	Super::DodgeEnd();

	ActionState = EActionState::EAS_Unoccupied;
}

void ACryptRaiderCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ACryptRaiderCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_EquippedTwoHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}


void ACryptRaiderCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ACryptRaiderCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void ACryptRaiderCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	/*UE_LOG(LogTemplateCharacter, Warning, TEXT("GetHit_Implementation"));*/
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
	
}

void ACryptRaiderCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
	//UE_LOG(LogTemplateCharacter, Warning, TEXT("HitReactEnd"));
}

void ACryptRaiderCharacter::UseHealthPotion()
{
	if (Attributes && PlayerOverlay)
	{
		Attributes->UseHealingPotion();
		PlayerOverlay->SetHealingPotion(Attributes->GetHealingPotion());
		PlayerOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

float ACryptRaiderCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}

void ACryptRaiderCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ACryptRaiderCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && PlayerOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		PlayerOverlay->SetSouls(Attributes->GetSouls());
	}
}

void ACryptRaiderCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && PlayerOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		PlayerOverlay->SetGold(Attributes->GetGold());
	}

}
void ACryptRaiderCharacter::AddHealingPotion(AHealingPotion* HealingPotion)
{
	if (Attributes && PlayerOverlay)
	{
		Attributes->AddHealingPotion(HealingPotion->GetHealingPotion());
		PlayerOverlay->SetHealingPotion(Attributes->GetHealingPotion());
	}
}
void ACryptRaiderCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}

}
