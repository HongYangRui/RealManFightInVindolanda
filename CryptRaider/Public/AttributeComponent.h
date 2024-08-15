// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRYPTRAIDER_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributeComponent();
	void UseHealingPotion();
	void ReduceStamina(float DeltaTime);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void RegenStamina(float DeltaTime);
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health = 100;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth = 100;
	// Current Stamina
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina = 100;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina = 100;
private:
	

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Gold;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Souls=5;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 HealingPotion=3;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost = 14.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 8.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float RunCost = 18.f;

public:
	bool ReceiveDamage(float Damage);
	float GetHealthPercent();
	bool IsAlive();
	void AddSouls(int32 NumberOfSouls);
	void AddGold(int32 AmountOfGold);
	void AddHealingPotion(int32 AmountOfHealingPotion);
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE int32 GetHealingPotion() const { return HealingPotion; }
	void UseStamina(float StaminaCost);
	float GetStaminaPercent();
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool PurchasePotion(int32 Cost, int32 PotionAmount);
};
