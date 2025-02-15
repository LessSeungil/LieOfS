// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/ABGA_Damaged.h"

UABGA_Damaged::UABGA_Damaged()
{
}

void UABGA_Damaged::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamagedEffect);
	if(EffectSpecHandle.IsValid())
	{
		//ApplyGameplayEffectSpecToTarget(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,EffectSpecHandle,targetData)
	}
		
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false,false);
}
