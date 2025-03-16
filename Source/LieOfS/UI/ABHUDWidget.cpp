// Fill out your copyright notice in the Description page of Project Settings.


#include "ABHUDWidget.h"
#include "LieOfS/Interface/ABCharacterHUDInterface.h"
#include "ABHpBarWidget.h"
#include "ABCharacterStatWidget.h"

UABHUDWidget::UABHUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UABHUDWidget::UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	FABCharacterStat TotalStat = BaseStat + ModifierStat;
	HpBar->UpdateStat(BaseStat, ModifierStat);

}

void UABHUDWidget::UpdateHpBar(AActor* Owner)
{
	HpBar->SetAbilitySystemComponent(Owner);
}