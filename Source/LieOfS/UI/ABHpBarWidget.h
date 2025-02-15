// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ABUserWidget.h"
#include "LieOfS/GameData/ABCharacterStat.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "ABHpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class LIEOFS_API UABHpBarWidget : public UABUserWidget, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	UABHpBarWidget(const FObjectInitializer& ObjectInitializer);
	

	
protected:
	virtual void NativeConstruct() override;

public:
	void UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat);
	void UpdateHpBar(float NewCurrentHp);
	FString GetHpStatText();
	void SetAbilitySystemComponent(AActor* InOwner);
	//Gas Section
protected:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void OnHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData);
	
protected:
	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

	UPROPERTY()
	TObjectPtr<class UTextBlock> HpStat;

	UPROPERTY()
	float CurrentHp;

	UPROPERTY()
	float MaxHp;

protected:
	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;
};
