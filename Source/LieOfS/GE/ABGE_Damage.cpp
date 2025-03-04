// Fill out your copyright notice in the Description page of Project Settings.


#include "GE/ABGE_Damage.h"
#include "Attribute/ABCharacterAttributeSet.h"

UABGE_Damage::UABGE_Damage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo HealthModifier;
	HealthModifier.Attribute = FGameplayAttribute(FindFieldChecked<FProperty>(UABCharacterAttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UABCharacterAttributeSet, Health)));
	HealthModifier.ModifierOp = EGameplayModOp::Additive;

	FScalableFloat DamageAmount(-30.0f);
	FGameplayEffectModifierMagnitude ModMagnitude(DamageAmount);

	HealthModifier.ModifierMagnitude = ModMagnitude;
	Modifiers.Add(HealthModifier);
}

void UABGE_Damage::OnExecuted(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const
{
	Super::OnExecuted(ActiveGEContainer, GESpec, PredictionKey);

	int a = 0;
}
