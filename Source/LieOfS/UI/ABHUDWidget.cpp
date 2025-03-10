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
	//HpBar->UpdateStat(BaseStat, ModifierStat);
	//CharacterStat->UpdateStat(BaseStat, ModifierStat);
}

void UABHUDWidget::UpdateHpBar(AActor* Owner)
{
	HpBar->SetAbilitySystemComponent(Owner);
}

void UABHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//HpBar = Cast<UABHpBarWidget>(GetWidgetFromName(TEXT("WidgetHpBar")));
	//ensure(HpBar);
	//
	//CharacterStat = Cast<UABCharacterStatWidget>(GetWidgetFromName(TEXT("WidgetCharacterStat")));
	//ensure(CharacterStat);
	//
	//IABCharacterHUDInterface* HUDPawn = Cast<IABCharacterHUDInterface>(GetOwningPlayerPawn());
	//if (HUDPawn)
	//{
	//	HUDPawn->SetupHUDWidget(this);
	//}

}
