// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/PlayerOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void UPlayerOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void UPlayerOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void UPlayerOverlay::SetGold(int32 Gold)
{
	if (GoldText)
	{
		const FString String = FString::Printf(TEXT("%d"), Gold);
		const FText Text = FText::FromString(String);
		GoldText->SetText(Text);
	}
}

void UPlayerOverlay::SetSouls(int32 Souls)
{
	if (SoulText)
	{
		const FString String = FString::Printf(TEXT("%d"), Souls);
		const FText Text = FText::FromString(String);
		SoulText->SetText(Text);
	}
}

void UPlayerOverlay::SetHealingPotion(int32 HealingPotion)
{
	if (HealingPotionText)
	{
		const FString String = FString::Printf(TEXT("%d"), HealingPotion);
		const FText Text = FText::FromString(String);
		HealingPotionText->SetText(Text);
	}

}
