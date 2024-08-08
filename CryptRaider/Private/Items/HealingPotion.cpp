// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/HealingPotion.h"
#include "Interfaces/PickupInterface.h"

void AHealingPotion::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->AddHealingPotion(this);
		SpawnPickupSystem();
		SpawnPickupSound();
		Destroy();
	}
}
