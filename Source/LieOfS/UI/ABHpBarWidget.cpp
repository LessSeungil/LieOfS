// Fill out your copyright notice in the Description page of Project Settings.


#include "ABHpBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "LieOfS/Interface/ABCharacterWidgetInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Attribute/ABCharacterAttributeSet.h"

UABHpBarWidget::UABHpBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxHp = -1.0f;
}

class UAbilitySystemComponent* UABHpBarWidget::GetAbilitySystemComponent() const
{
	return ASC;
}

void UABHpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHpBar")));
	ensure(HpProgressBar);

	HpStat = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtHpStat")));
	ensure(HpStat);

	IABCharacterWidgetInterface* CharacterWidget = Cast<IABCharacterWidgetInterface>(OwningActor);
	if (CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
}

void UABHpBarWidget::UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	MaxHp = (BaseStat + ModifierStat).MaxHp;

	if (HpProgressBar)
	{
		HpProgressBar->SetPercent(CurrentHp / MaxHp);
	}

	if (HpStat)
	{
		HpStat->SetText(FText::FromString(GetHpStatText()));
	}
}

void UABHpBarWidget::UpdateHpBar(float NewCurrentHp)
{
	CurrentHp = NewCurrentHp;

	ensure(MaxHp > 0.0f);
	if (HpProgressBar)
	{
		HpProgressBar->SetPercent(CurrentHp / MaxHp);
	}

	if (HpStat)
	{
		HpStat->SetText(FText::FromString(GetHpStatText()));
	}
}

FString UABHpBarWidget::GetHpStatText()
{
	return FString::Printf(TEXT("%.0f/%0.f"), CurrentHp, MaxHp);
}

void UABHpBarWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	if(InOwner)
	{
		ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner);
		if(ASC)
		{
			ASC->GetGameplayAttributeValueChangeDelegate(UABCharacterAttributeSet::GetHealthAttribute()).AddUObject(this, &UABHpBarWidget::OnHealthChanged);
			ASC->GetGameplayAttributeValueChangeDelegate(UABCharacterAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UABHpBarWidget::OnMaxHealthChanged);

			const UABCharacterAttributeSet* CurrentAttributeSet = ASC->GetSet<UABCharacterAttributeSet>();
			if(CurrentAttributeSet)
			{
				MaxHp = CurrentAttributeSet->GetMaxHealth();
				UpdateHpBar(CurrentAttributeSet->GetHealth());
			}
			
		}
	}
}

void UABHpBarWidget::OnHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	UpdateHpBar(ChangeData.NewValue);
}

void UABHpBarWidget::OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	MaxHp = ChangeData.NewValue;
	UpdateHpBar(CurrentHp);
}
