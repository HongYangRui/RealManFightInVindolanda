// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "HealingPotion.generated.h"

/**
 * 
 */
UCLASS()
class CRYPTRAIDER_API AHealingPotion : public AItem
{
	GENERATED_BODY()
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
private:
	UPROPERTY(EditAnywhere, Category = "HealingPotion Properties")
	int32 HealingPotion;
public:
	FORCEINLINE int32 GetHealingPotion() const { return HealingPotion; }
	FORCEINLINE void SetHealingPotion(int32 NumberOfHealingPotion) { HealingPotion = NumberOfHealingPotion; }
	
};
