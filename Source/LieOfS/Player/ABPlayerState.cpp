// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ABPlayerState.h"
#include "AbilitySystemComponent.h"

AABPlayerState::AABPlayerState()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
}

class UAbilitySystemComponent* AABPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
